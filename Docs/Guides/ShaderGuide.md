# Shader.h / Shader.cpp 理解指南

这份文档用于理解当前的：

```text
src/Engine/Renderer/Shader.h
src/Engine/Renderer/Shader.cpp
```

它们的目标不是“直接画图”，而是把 OpenGL Shader Program 封装成一个 C++ 对象。以后 Layer、Renderer2D 或材质系统需要绘制时，只需要使用这个对象，不需要每次手写一整套 OpenGL 编译和链接流程。

## 1. Shader 在引擎里的位置

当前调用关系可以先这样理解：

```text
Sandbox Layer
    -> Shader::Bind()
    -> Shader::SetMat4() / Shader::SetFloat4()
    -> 后续 VertexArray / DrawCall
    -> OpenGL 使用当前 Shader Program 绘制
```

也就是说，`Shader` 属于 Renderer 层。它不负责窗口、不负责帧循环、不负责顶点数据，只负责一件事：

```text
管理一个 OpenGL shader program 的生命周期和使用方式。
```

对应源码里的职责注释：

```cpp
// Owns one OpenGL shader program compiled from vertex/fragment GLSL files.
// Flow: Sandbox/Renderer2D -> Shader::Bind -> OpenGL program state.
class Shader
```

这句话的意思是：

- `Shader` 拥有一个 OpenGL program id。
- 这个 program id 存在 `m_RendererID` 里。
- Sandbox 或未来的 Renderer2D 会调用 `Bind()`。
- `Bind()` 后，OpenGL 后续绘制会使用这个 program。

## 2. Shader.h 应该先看什么

`Shader.h` 是对外接口，应用层和其他渲染模块主要看这里。

### 构造函数

```cpp
Shader(const std::string& vertexPath, const std::string& fragmentPath);
```

它需要两个文件路径：

```text
vertex shader 文件路径
fragment shader 文件路径
```

例如以后可能这样用：

```cpp
TinyTactics::Shader shader(
    "Assets/Shaders/FlatColor.vert",
    "Assets/Shaders/FlatColor.frag");
```

构造函数内部会完成：

```text
读取 vertex 文件
读取 fragment 文件
编译 vertex shader
编译 fragment shader
链接 OpenGL program
删除临时 shader object
保存 program id 到 m_RendererID
```

所以创建 `Shader` 对象时，OpenGL shader program 就已经准备好了。

### 析构函数

```cpp
~Shader();
```

析构函数会释放 OpenGL program。

这就是 RAII 思路：

```text
C++ 对象创建 -> OpenGL 资源创建
C++ 对象销毁 -> OpenGL 资源释放
```

你以后不需要手动记住 `glDeleteProgram()`，`Shader` 会自己处理。

### 禁止拷贝

```cpp
Shader(const Shader&) = delete;
Shader& operator=(const Shader&) = delete;
```

这里禁止拷贝很重要。

原因是 `Shader` 里有一个 OpenGL program id：

```cpp
uint32_t m_RendererID = 0;
```

如果允许拷贝，就会出现两个 C++ 对象指向同一个 OpenGL program。两个对象析构时都会释放同一个 id，导致重复释放。

所以它不能拷贝。

### 允许移动

```cpp
Shader(Shader&& other) noexcept;
Shader& operator=(Shader&& other) noexcept;
```

移动的意思是：

```text
把 OpenGL program 所有权从一个 Shader 对象转移到另一个 Shader 对象。
```

这适合以后把 `Shader` 放进容器、资源管理器，或者从函数返回。

移动后，旧对象的 `m_RendererID` 会变成 `0`，避免旧对象析构时误删资源。

### Bind / Unbind

```cpp
void Bind() const;
static void Unbind();
```

OpenGL 是状态机。`Bind()` 的意思是：

```text
让 OpenGL 后续 draw call 使用这个 shader program。
```

对应 cpp：

```cpp
glUseProgram(m_RendererID);
```

`Unbind()` 则是：

```cpp
glUseProgram(0);
```

实际使用时一般是：

```cpp
shader.Bind();
shader.SetFloat4("u_Color", color);
// 后续提交顶点并 draw
```

注意：设置 uniform 通常应该在 `Bind()` 之后做。

### SetMat4 / SetFloat4

```cpp
void SetMat4(const std::string& name, const glm::mat4& value) const;
void SetFloat4(const std::string& name, const glm::vec4& value) const;
```

这两个函数用于给 GLSL 里的 uniform 传值。

例如 GLSL 里有：

```glsl
uniform mat4 u_ViewProjection;
uniform vec4 u_Color;
```

C++ 里就可以写：

```cpp
shader.Bind();
shader.SetMat4("u_ViewProjection", viewProjection);
shader.SetFloat4("u_Color", color);
```

`glm::mat4` 和 `glm::vec4` 是数学类型。OpenGL 本身不认识 C++ 对象，所以 cpp 里用了：

```cpp
glm::value_ptr(value)
```

它会把 glm 对象转换成 OpenGL 能读取的连续内存指针。

## 3. Shader.cpp 的执行流程

`Shader.cpp` 是具体实现。最重要的是构造函数。

### 第一步：读取文件

```cpp
const std::string vertexSource = ReadFile(vertexPath);
const std::string fragmentSource = ReadFile(fragmentPath);
```

`ReadFile()` 做的事情是：

```text
打开文件
读取全部内容到 string
返回 GLSL 源码字符串
```

如果文件打不开，会抛异常：

```cpp
throw std::runtime_error("Failed to open shader file: " + path);
```

所以如果路径写错，程序会直接告诉你哪个 shader 文件打不开。

### 第二步：编译单个 shader

```cpp
const uint32_t vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
const uint32_t fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);
```

OpenGL 里 vertex shader 和 fragment shader 要分别编译。

`CompileShader()` 内部流程是：

```text
glCreateShader()
glShaderSource()
glCompileShader()
glGetShaderiv(... GL_COMPILE_STATUS ...)
失败时读取错误日志
成功时返回 shader id
```

如果 GLSL 语法写错，这里会抛异常：

```cpp
throw std::runtime_error("Failed to compile shader '" + path + "':\n" + log);
```

这比静默失败好很多，因为你能直接看到 GLSL 编译日志。

### 第三步：链接 program

```cpp
m_RendererID = glCreateProgram();
glAttachShader(m_RendererID, vertexShader);
glAttachShader(m_RendererID, fragmentShader);
glLinkProgram(m_RendererID);
```

OpenGL shader object 编译成功以后，还不能直接用于绘制。必须链接成一个 program。

可以理解成：

```text
vertex shader + fragment shader -> shader program
```

`m_RendererID` 保存的就是这个 shader program 的 id。

### 第四步：检查链接结果

```cpp
glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
```

如果链接失败，会读取 program link log，然后清理资源：

```cpp
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);
Release();
```

最后抛异常。

链接失败常见原因：

- vertex shader 输出和 fragment shader 输入不匹配。
- GLSL 版本不一致。
- uniform/block 定义冲突。

### 第五步：删除临时 shader object

```cpp
glDetachShader(m_RendererID, vertexShader);
glDetachShader(m_RendererID, fragmentShader);
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);
```

这里容易误解。

链接成功后，OpenGL program 已经保存了最终链接结果。原来的 vertex shader object 和 fragment shader object 不再需要保留。

所以删除它们是正确的，不会破坏 program。

## 4. 资源释放为什么写 Release()

```cpp
void Shader::Release()
{
    if (m_RendererID != 0)
    {
        glDeleteProgram(m_RendererID);
        m_RendererID = 0;
    }
}
```

`Release()` 做两件事：

```text
如果 program id 有效，就删除 OpenGL program。
删除后把 id 置 0。
```

置 0 很重要，因为这样重复调用 `Release()` 不会重复删除同一个 OpenGL 对象。

它会被这些地方调用：

```text
析构函数
move 赋值前
链接失败清理
```

这就是把资源释放逻辑集中到一个函数里，避免每个地方都手写 `glDeleteProgram()`。

## 5. move 构造和 move 赋值在做什么

move 构造：

```cpp
Shader::Shader(Shader&& other) noexcept
    : m_RendererID(std::exchange(other.m_RendererID, 0))
{
}
```

意思是：

```text
把 other 的 OpenGL program id 拿过来。
同时把 other.m_RendererID 改成 0。
```

move 赋值：

```cpp
Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        Release();
        m_RendererID = std::exchange(other.m_RendererID, 0);
    }

    return *this;
}
```

意思是：

```text
先释放自己原来的 OpenGL program。
再接管 other 的 OpenGL program。
最后把 other 清空。
```

这就是“资源所有权转移”。

## 6. uniform 设置流程

以 `SetMat4()` 为例：

```cpp
void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}
```

流程是：

```text
通过 uniform 名字查 location
把 glm::mat4 转成 float 指针
调用 glUniformMatrix4fv 上传给当前 program
```

`SetFloat4()` 类似，只是上传的是 vec4：

```cpp
glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
```

注意一个 OpenGL 规则：

```text
glUniform* 修改的是当前绑定的 program。
```

所以使用顺序应该是：

```cpp
shader.Bind();
shader.SetMat4("u_ViewProjection", viewProjection);
shader.SetFloat4("u_Color", color);
```

## 7. 现在这个 Shader 还缺什么

当前版本已经能跑通基础 shader program，但它还不是完整引擎级 Shader 系统。

后续可以继续补：

```text
1. uniform location 缓存
2. 从一个 shader 文件中同时解析 vertex/fragment
3. 支持 int、float、vec2、vec3、texture sampler
4. 支持 shader reload
5. 支持 debug name
6. 支持 RendererAPI 抽象，避免 Shader 直接固定 OpenGL
```

其中最先建议补的是：

```text
uniform location 缓存
```

因为现在每次 `SetMat4()` / `SetFloat4()` 都会调用：

```cpp
glGetUniformLocation()
```

这个查找可以缓存起来，减少每帧重复查找。

## 8. 在 BaseLayer 里怎么用

未来你可以在 Sandbox Layer 里大概这样用：

```cpp
class BaseLayer final : public TinyTactics::Layer
{
public:
    void OnAttach() override
    {
        m_Shader = std::make_unique<TinyTactics::Shader>(
            "Assets/Shaders/FlatColor.vert",
            "Assets/Shaders/FlatColor.frag");
    }

    void OnUpdate(TinyTactics::TimeStep timestep) override
    {
        m_Shader->Bind();
        m_Shader->SetFloat4("u_Color", glm::vec4(1.0F, 0.2F, 0.1F, 1.0F));

        // 后面会接 VertexArray / RenderCommand::DrawIndexed。
    }

private:
    std::unique_ptr<TinyTactics::Shader> m_Shader;
};
```

这段代码目前还不能单独画出图形，因为画图还需要：

```text
VertexBuffer
IndexBuffer
VertexArray
Draw call
```

但它展示了 Shader 在 Layer 中的使用位置。

## 9. 推荐阅读顺序

如果你想理解这两个文件，建议按这个顺序看：

```text
1. Shader.h 的 class 注释
2. 构造函数声明
3. Bind / SetMat4 / SetFloat4
4. Shader.cpp 构造函数
5. CompileShader()
6. Release()
7. move 构造和 move 赋值
```

不要一上来陷进 OpenGL 所有函数。先记住主线：

```text
读取 GLSL -> 编译 shader object -> 链接 program -> Bind 使用 -> 析构释放
```

这就是当前 `Shader` 封装的核心。
