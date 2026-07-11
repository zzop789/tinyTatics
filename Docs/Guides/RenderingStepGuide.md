# TinyTactics 渲染系统入门指南

这份指南从当前项目状态开始：引擎已经能创建窗口、初始化 OpenGL Context、清屏、交换缓冲区。下一步目标是让应用层不写裸 OpenGL，也能绘制图形、载入图片、播放简单 2D 动画。

## 0. 当前调用关系

现在的代码路径是：

```text
src/Sandbox/main.cpp
    -> TinyTactics::Application
        -> TinyTactics::Window
            -> GLFW 窗口 + OpenGL Context + GLAD
        -> TinyTactics::RenderCommand
            -> glClearColor / glClear
```

当前应用层代码只做这件事：

```cpp
TinyTactics::Application application({
    "TinyTactics Sandbox",
    1280,
    720,
    true,
});

application.Run();
```

这对第一个引擎里程碑是正确的。现在缺的是一层“渲染接口”，它应该站在裸 OpenGL 上面，让应用层通过方法调用来画东西。

最终应用层应该能这样写：

```cpp
Renderer2D::BeginScene(camera);
Renderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.2f, 0.2f, 1.0f});
Renderer2D::DrawSprite({2.0f, 0.0f}, {1.0f, 1.0f}, texture);
Renderer2D::EndScene();
```

## 1. 总路线

按这个顺序做：

```text
1. 添加 Layer，让游戏代码每帧有 OnUpdate 入口
2. 添加 Shader
3. 添加 VertexBuffer / IndexBuffer / VertexArray
4. 先画出一个彩色矩形
5. 添加 Camera2D
6. 添加 Renderer2D::DrawQuad
7. 使用 stb_image 添加 Texture2D
8. 添加 Renderer2D::DrawSprite
9. 添加 SpriteAnimation
10. 添加批渲染和渲染统计
```

不要一开始就做贴图动画。先证明一个三角形或矩形能画出来，再封装它。

## 2. 第一步：添加 Layer

现在 `Application::Run()` 拥有主循环，但游戏层没有地方每帧执行逻辑。

需要添加：

```text
src/Engine/Core/Layer.h
src/Engine/Core/Layer.cpp
src/Engine/Core/LayerStack.h
src/Engine/Core/LayerStack.cpp
```

最小接口：

```cpp
class Layer
{
public:
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(TimeStep timestep) {}
};
```

然后让 `Application` 持有一个 `LayerStack`，并在每帧调用：

```text
Application::Run
    -> 遍历每个 Layer
        -> Layer::OnUpdate
```

目标应用层写法：

```cpp
class SandboxLayer : public TinyTactics::Layer
{
public:
    void OnUpdate(TinyTactics::TimeStep timestep) override
    {
        // 游戏逻辑或绘制测试代码写在这里。
    }
};
```

这一步的意义：

- `Application` 保持在引擎层。
- `SandboxLayer` 变成应用层。
- 渲染 Demo、战斗逻辑、编辑器面板以后都不用塞进 `Application::Run()`。

## 3. 第二步：添加 Shader

OpenGL Core Profile 不能不写 Shader 就画东西。

需要添加：

```text
src/Engine/Renderer/Shader.h
src/Engine/Renderer/Shader.cpp
Assets/Shaders/FlatColor.vert
Assets/Shaders/FlatColor.frag
```

`Shader` 的职责：

```text
读取 GLSL 源码
编译 vertex shader
编译 fragment shader
链接 program
Bind / Unbind
设置 uniform
输出编译错误
```

第一个 vertex shader：

```glsl
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;

out vec4 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
```

第一个 fragment shader：

```glsl
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec4 v_Color;

void main()
{
    o_Color = v_Color;
}
```

验证标准：

- Shader 编译失败时能打印 GLSL 错误日志。
- Shader 编译成功后，再继续做 Buffer。

## 4. 第三步：添加 Buffer 和 VertexArray

一个矩形需要顶点数据和索引数据。

需要添加：

```text
src/Engine/Renderer/VertexBuffer.h
src/Engine/Renderer/VertexBuffer.cpp
src/Engine/Renderer/IndexBuffer.h
src/Engine/Renderer/IndexBuffer.cpp
src/Engine/Renderer/VertexArray.h
src/Engine/Renderer/VertexArray.cpp
src/Engine/Renderer/BufferLayout.h
```

第一版矩形数据可以这样写：

```cpp
float vertices[] = {
    // position           // color
    -0.5f, -0.5f, 0.0f,   1.0f, 0.2f, 0.2f, 1.0f,
     0.5f, -0.5f, 0.0f,   0.2f, 1.0f, 0.2f, 1.0f,
     0.5f,  0.5f, 0.0f,   0.2f, 0.2f, 1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f
};

uint32_t indices[] = {
    0, 1, 2,
    2, 3, 0
};
```

Buffer 之间的关系：

```text
VertexArray
    保存顶点属性布局
    引用 VertexBuffer
    引用 IndexBuffer
```

验证标准：

- Bind shader。
- Bind vertex array。
- 调用 `glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)`。
- 期望结果：屏幕上出现一个彩色矩形。

这一阶段可以在测试 Layer 里临时写裸 OpenGL，因为目标是证明底层封装可用。过了这一步，就应该把裸 OpenGL 移到 `Renderer2D` 后面。

## 5. 第四步：添加 Camera2D

没有摄像机，坐标会很难管理。

需要添加：

```text
src/Engine/Renderer/Camera2D.h
src/Engine/Renderer/Camera2D.cpp
```

最小 API：

```cpp
class Camera2D
{
public:
    void SetViewportSize(float width, float height);
    void SetPosition(const glm::vec2& position);
    void SetZoom(float zoom);

    const glm::mat4& GetViewProjection() const;

private:
    void Recalculate();
};
```

Shader 需要接收：

```cpp
shader.SetMat4("u_ViewProjection", camera.GetViewProjection());
```

验证标准：

- 修改 camera position，确认矩形相对视口发生移动。
- 缩放窗口，确认投影仍然正确。

## 6. 第五步：添加 Renderer2D::DrawQuad

现在开始隐藏裸 OpenGL 绘制路径。

需要添加：

```text
src/Engine/Renderer/Renderer2D.h
src/Engine/Renderer/Renderer2D.cpp
```

第一版 API：

```cpp
class Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const Camera2D& camera);
    static void EndScene();

    static void DrawQuad(const glm::vec2& position,
                         const glm::vec2& size,
                         const glm::vec4& color);
};
```

调用关系：

```text
SandboxLayer::OnUpdate
    -> Renderer2D::BeginScene(camera)
    -> Renderer2D::DrawQuad(...)
    -> Renderer2D::EndScene()
        -> OpenGL draw call
```

目标应用层代码：

```cpp
void SandboxLayer::OnUpdate(TinyTactics::TimeStep timestep)
{
    Renderer2D::BeginScene(m_Camera);
    Renderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.2f, 0.2f, 1.0f});
    Renderer2D::EndScene();
}
```

验证标准：

- 应用层不 include `glad/gl.h`。
- 应用层不调用 `glDrawElements`。
- 屏幕上出现一个彩色矩形。

## 7. 第六步：添加 Texture2D

图片加载使用 `stb_image`。

需要添加：

```text
src/Engine/Renderer/Texture2D.h
src/Engine/Renderer/Texture2D.cpp
Assets/Textures/
```

最小 API：

```cpp
class Texture2D
{
public:
    explicit Texture2D(const std::string& path);
    ~Texture2D();

    void Bind(uint32_t slot = 0) const;

    int GetWidth() const;
    int GetHeight() const;
};
```

图片加载流程：

```text
Texture2D(path)
    -> stbi_load
    -> glCreateTextures / glTexImage2D
    -> glGenerateMipmap 或设置过滤方式
    -> stbi_image_free
```

贴图 shader 需要：

```glsl
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

uniform sampler2D u_Texture;
```

验证标准：

- 加载一张 PNG。
- 把它画到一个矩形上。
- 如果图片上下颠倒，使用 `stbi_set_flip_vertically_on_load(true)`。

## 8. 第七步：添加 Renderer2D::DrawSprite

`Texture2D` 可用后，再把贴图绘制加入 `Renderer2D`。

API：

```cpp
static void DrawSprite(const glm::vec2& position,
                       const glm::vec2& size,
                       const std::shared_ptr<Texture2D>& texture,
                       const glm::vec4& tintColor = glm::vec4(1.0f));
```

调用关系：

```text
SandboxLayer::OnAttach
    -> 只加载一次 Texture2D

SandboxLayer::OnUpdate
    -> Renderer2D::DrawSprite(texture)
```

目标应用层代码：

```cpp
void SandboxLayer::OnAttach()
{
    m_Texture = std::make_shared<Texture2D>("Assets/Textures/unit.png");
}

void SandboxLayer::OnUpdate(TinyTactics::TimeStep timestep)
{
    Renderer2D::BeginScene(m_Camera);
    Renderer2D::DrawSprite({0.0f, 0.0f}, {1.0f, 1.0f}, m_Texture);
    Renderer2D::EndScene();
}
```

验证标准：

- 窗口里出现一张图片。
- 应用层仍然不 include 裸 OpenGL 头文件。

## 9. 第八步：添加 Sprite Animation

静态图片渲染成功后，再做动画。

常见方案有两个：

```text
方案 A：多张帧图片
    idle_0.png
    idle_1.png
    idle_2.png

方案 B：Sprite Sheet
    一张大图里包含多个动画帧
```

建议先做方案 A，因为更容易验证。

需要添加：

```text
src/Engine/Renderer/SpriteAnimation.h
src/Engine/Renderer/SpriteAnimation.cpp
```

最小 API：

```cpp
class SpriteAnimation
{
public:
    void AddFrame(std::shared_ptr<Texture2D> texture, float duration);
    void Update(TimeStep timestep);

    const std::shared_ptr<Texture2D>& GetCurrentTexture() const;
};
```

调用关系：

```text
SandboxLayer::OnUpdate
    -> animation.Update(timestep)
    -> Renderer2D::DrawSprite(animation.GetCurrentTexture())
```

验证标准：

- 加载 2 到 4 张帧图片。
- 确认图片会随时间切换。
- 后面再把多图帧动画替换成 Sprite Sheet + UV 动画。

## 10. 第九步：添加 Renderer Stats

在做更大的游戏场景前，先加统计数据。

可以先定义：

```cpp
struct RendererStats
{
    uint32_t drawCalls = 0;
    uint32_t quadCount = 0;
};
```

暴露接口：

```cpp
Renderer2D::ResetStats();
Renderer2D::GetStats();
```

验证标准：

- 画一个 Quad。
- 期望 `quadCount = 1`。
- 批渲染前，期望 `drawCalls = 1`。

这会为后面的 ImGui 面板和批渲染前后对比做准备。

## 11. 现在不要急着做什么

不要一上来就做：

```text
完整 Batch Renderer
Texture Slot 数组
FrameBuffer 编辑器视口
ImGui 场景编辑器
ECS RenderSystem
```

这些都重要，但它们依赖前面的基础能力。

正确推进顺序是：

```text
窗口能打开
    -> 画一个矩形
        -> 画一张图片
            -> 播放一个 Sprite 动画
                -> 画很多 Sprite
                    -> 做批渲染
```

## 12. 推荐下一个编码任务

下一个最具体的实现任务应该是：

```text
添加 Layer + Shader + Buffer 封装，然后从 SandboxLayer 里画出一个彩色矩形。
```

这一步能证明：

- `Application` 可以每帧调用应用层代码。
- Shader 编译和链接是正常的。
- VAO / VBO / EBO 封装是正常的。
- OpenGL 绘制调用开始被藏到渲染抽象后面。

完成这一步后，再实现 `Texture2D` 和 `Renderer2D::DrawSprite`。
