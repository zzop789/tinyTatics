# 关闭窗口时报 Heap Corruption 的问题复盘

这份文档记录这次 `TinyTacticsSandbox.exe` 在关闭窗口时弹出 MSVC Debug CRT 错误的成因、排查思路和修复关键点。

## 1. 现象

关闭窗口时弹出：

```text
HEAP CORRUPTION DETECTED: after Normal block
CRT detected that the application wrote to memory after end of heap buffer.
```

这个报错不是“内存不够”，也不是普通意义上的“忘记释放资源”。

它的真实含义是：

```text
某处代码写出了堆内存块的结尾。
Debug CRT 在释放或检查堆时发现尾部保护区被破坏。
```

它在关闭窗口时出现，是因为程序退出时会触发：

```text
Application 析构
LayerStack 析构
Layer 析构
Shader 析构
Window 析构
GLFW/OpenGL 资源释放
CRT 堆检查
```

所以关闭窗口只是“发现问题的时间点”，不一定是“写坏内存的时间点”。

## 2. 为什么先怀疑资源回收链路

当前项目里关闭窗口的核心链路是：

```text
用户点击 X
    -> GLFW 标记窗口 ShouldClose
    -> Application::Run 退出 while
    -> Application::~Application
    -> LayerStack::Clear
    -> BaseLayer::OnDetach
    -> Shader::~Shader / Shader::Release
    -> Window::~Window / Window::Shutdown
    -> glfwDestroyWindow / glfwTerminate
```

这里有两个高风险点：

```text
1. OpenGL 资源必须在 OpenGL context 还活着时释放。
2. GLFW callback/user pointer 不能在 Window 对象销毁后继续指向旧地址。
```

如果顺序不清楚，就可能出现：

```text
Window/OpenGL context 先没了，Shader 后释放。
GLFW callback 还拿着旧 user pointer。
Layer 资源释放依赖隐式成员析构顺序，不够直观。
```

这些问题不一定每次都立刻崩，但在 Debug CRT 下关闭程序时很容易暴露。

## 3. 修复关键点一：Application 显式控制关闭顺序

之前主要依赖成员变量的析构顺序。虽然 C++ 成员析构有固定规则，但引擎层建议不要把跨系统资源释放完全藏在隐式顺序里。

现在 `Application::~Application()` 显式写成：

```cpp
Application::~Application()
{
    // Flow: Application shutdown -> Layers release GL resources -> Window destroys GL context.
    m_LayerStack.Clear();
    m_Window.reset();
}
```

含义：

```text
先让所有 Layer 释放自己的资源。
再销毁 Window。
最后才销毁 OpenGL context / GLFW window。
```

这对渲染引擎很重要，因为很多 Layer 以后都会拥有 GPU 资源：

```text
Shader
Texture
VertexBuffer
IndexBuffer
VertexArray
Framebuffer
```

这些资源都应该在 context 有效时释放。

## 4. 修复关键点二：LayerStack 增加 Clear()

`LayerStack` 现在提供：

```cpp
void LayerStack::Clear()
{
    // Flow: Application shutdown -> newest Layer::OnDetach first -> owned resources release.
    for (auto iterator = m_Layers.rbegin(); iterator != m_Layers.rend(); ++iterator)
    {
        (*iterator)->OnDetach();
    }

    m_Layers.clear();
}
```

这里有两个设计点：

```text
1. 先调用 OnDetach，再销毁 Layer。
2. 倒序释放 Layer。
```

倒序释放是常见资源栈规则：

```text
后创建的资源，通常先释放。
```

以后如果有：

```text
EditorLayer
GameLayer
ImGuiLayer
```

倒序释放更容易保证依赖关系安全。

## 5. 修复关键点三：BaseLayer 显式释放 Shader

`BaseLayer` 拥有：

```cpp
std::unique_ptr<TinyTactics::Shader> m_Shader;
```

现在在 `OnDetach()` 中显式释放：

```cpp
void BaseLayer::OnDetach()
{
    // Flow: Application shutdown -> LayerStack::OnDetach -> release OpenGL-owned Shader.
    m_Shader.reset();
}
```

这样 Shader 不会等到 `BaseLayer` 析构时才释放，而是在引擎规定的 Layer detach 阶段释放。

这个结构更适合以后扩展，因为 Layer 的生命周期变成：

```text
OnAttach 申请资源
OnUpdate 使用资源
OnDetach 释放资源
析构函数只做最后兜底
```

## 6. 修复关键点四：Window 销毁前解绑 GLFW 状态

`Window::Shutdown()` 现在销毁 native window 前先解绑：

```cpp
glfwSetFramebufferSizeCallback(m_Handle, nullptr);
glfwSetWindowUserPointer(m_Handle, nullptr);
glfwMakeContextCurrent(m_Handle);
glfwDestroyWindow(m_Handle);
```

原因：

```text
GLFW callback 和 user pointer 是 C 风格状态。
它们不会自动知道 C++ Window 对象是否还活着。
销毁前主动清空，可以减少回调访问旧对象的风险。
```

同时，`glfwMakeContextCurrent(m_Handle)` 保证销毁窗口前当前 context 是这个窗口的 context。

最后一个窗口销毁时：

```cpp
glfwMakeContextCurrent(nullptr);
glfwTerminate();
```

这样 GLFW 退出时不会还保留一个已经销毁的 current context。

## 7. 修复关键点五：GLFW user pointer 指向 Window 本身

之前 user pointer 指向的是：

```cpp
&m_Specification
```

现在改成：

```cpp
glfwSetWindowUserPointer(m_Handle, this);
```

回调中再取回：

```cpp
auto* engineWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
engineWindow->m_Specification.width = static_cast<uint32_t>(width);
engineWindow->m_Specification.height = static_cast<uint32_t>(height);
```

这样更清晰：

```text
GLFW window -> Engine Window 对象 -> Window 内部状态
```

以后增加更多回调时也不需要反复把 user pointer 解释成不同类型。

## 8. 修复关键点六：Shader 日志读取使用安全缓冲区

`Shader.cpp` 里 OpenGL 编译/链接失败时会读取错误日志：

```cpp
glGetShaderInfoLog(...)
glGetProgramInfoLog(...)
```

这些函数会往调用者提供的缓冲区写数据。为避免驱动写入结尾字符时踩到边界，改成：

```cpp
std::vector<char> log(static_cast<size_t>(logLength > 0 ? logLength : 1) + 1, '\0');
```

重点是：

```text
多留 1 个字节。
由 OpenGL 写入 vector 的 data。
再根据实际 writtenLength 构造 std::string。
```

这类代码属于典型的 C API 与 C++ 容器交界处，高风险。

## 9. 为什么增加 --auto-close

为了验证关闭链路，新增了一个测试入口：

```powershell
.\build\default\TinyTacticsSandbox.exe --auto-close=0.5
```

它会让程序运行 0.5 秒后自动走正常关闭流程。

用途：

```text
不靠人工点窗口。
每次都走相同的关闭路径。
方便重复验证析构链。
```

验证时使用了两种路径：

```text
1. --auto-close 自动退出
2. 模拟窗口关闭消息
```

最终连续多次退出码都是：

```text
0
```

说明当前关闭链路已经稳定。

## 10. 遇到同类问题的排查顺序

以后再遇到关闭窗口时报 heap corruption，可以按这个顺序查：

```text
1. 确认是不是旧 exe / 旧进程还在运行。
2. 确认 Debug CRT 报的是 heap corruption，不是普通 exception。
3. 查关闭顺序：Application -> LayerStack -> Layer -> GPU resource -> Window。
4. 查 OpenGL 资源是否在 context 销毁前释放。
5. 查 GLFW callback / user pointer 是否指向已经销毁的对象。
6. 查所有 C API 写入缓冲区的位置。
7. 增加自动关闭参数，重复运行验证。
8. 如果还复现，用 VS 调试器点 Retry 看调用栈。
```

特别要注意这些代码形态：

```text
glGet*InfoLog 写入 string/vector
glfwSetWindowUserPointer 指向内部对象
unique_ptr 资源释放依赖隐式析构顺序
回调里 static_cast 成错误类型
窗口销毁后还调用 glDelete*
```

## 11. 这次学到的规则

对引擎代码来说，资源生命周期必须显式。

推荐结构：

```text
Application 控制系统级关闭顺序。
LayerStack 控制 Layer 生命周期。
Layer::OnAttach 创建资源。
Layer::OnDetach 释放资源。
Window 最后销毁 OpenGL context。
```

一句话总结：

```text
OpenGL 资源先退场，Window/context 后退场。
```

这比单纯依赖 C++ 默认析构顺序更适合游戏引擎。
