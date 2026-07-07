# OpenGL / C++ 求职项目参考方案：TinyTactics Engine

## 0. 项目定位

本项目目标不是做一个简单的 OpenGL 教程 Demo，也不是单纯做一个炫技渲染器，而是做一个能够证明游戏客户端工程能力的小型项目。

推荐方向：

> 使用 C++17 / OpenGL 实现一个小型 2D / 2.5D 策略战斗沙盒 Demo，展示渲染、玩法系统、调试工具、数据驱动和性能优化能力。

项目关键词：

* C++17
* OpenGL 3.3 Core Profile
* 游戏客户端
* 轻量 ECS-like 架构
* 2D Batch Renderer
* A* 寻路
* 自动战斗
* 数据驱动配置
* ImGui 调试工具
* 性能 Profiler
* 对象池
* Fixed Timestep
* DebugDraw
* FrameBuffer
* RenderDoc / OpenGL Debug

最终目标：

> 我不是只会写一个 OpenGL 玩具 Demo，而是能用 C++ 和 OpenGL 搭出一个具备渲染、玩法、调试、配置和性能分析能力的小型游戏技术栈。

---

# 1. 项目名称建议

可以使用以下名称之一：

* TinyTactics Engine
* OpenGL Tactical Sandbox
* Mini Strategy Engine
* ZEngine Tactics Demo
* Tiny Battle Engine

---

# 2. 项目核心目标

项目需要证明以下能力：

| 能力          | 项目体现方式                                                  |
| ----------- | ------------------------------------------------------- |
| C++ 工程能力    | RAII、智能指针、模块解耦、CMake、资源管理                               |
| OpenGL 渲染能力 | Shader、Texture、Buffer、Camera、FrameBuffer、Batch Renderer |
| 游戏开发能力      | 主循环、输入、寻路、战斗、技能、Buff、投射物                                |
| 架构能力        | Engine 层 / Game 层分离，轻量 ECS-like 架构                      |
| 工具能力        | ImGui 编辑器、Debug 面板、Log、Profiler                         |
| 性能意识        | Draw Call 统计、对象池、批渲染、帧耗时统计                              |
| 可维护性        | README、架构图、清晰目录结构、注释、Git 提交记录                           |
| 可展示性        | GIF、视频、可下载 exe、截图、技术说明文档                                |

---

# 3. 推荐项目形态

做一个俯视角策略战斗 Demo。

基础玩法：

* 左侧刷出敌人
* 右侧是防守区域
* 玩家可以放置单位
* 单位自动寻敌
* 单位自动攻击
* 支持子弹 / 技能 / AOE
* 支持血条、选中框、攻击范围显示
* 支持简单地图编辑
* 支持 JSON 配置单位和技能
* 支持 ImGui 调试面板

画面可以简单，重点是系统完整。

---

# 4. 架构原则

## 4.1 不做纯 OOP

不推荐把所有对象都做成继承类：

```cpp
class Unit;
class Soldier : public Unit;
class Archer : public Unit;
class Mage : public Unit;
class Tower : public Unit;
class Projectile : public GameObject;
```

这种方式容易出现继承爆炸，不利于扩展单位、子弹、Buff、技能和建筑。

---

## 4.2 使用轻量 ECS-like 架构

推荐使用轻量 ECS-like 架构，而不是完整工业级 ECS。

核心思想：

```text
Entity = ID
Component = 数据
System = 逻辑
World = 管理所有实体和组件
```

不要一开始实现复杂功能：

* Archetype
* Sparse Set
* 反射系统
* 高级 Query
* 多线程 Job System
* 复杂生命周期管理

第一版只需要简单、可运行、好调试。

---

## 4.3 游戏逻辑不要直接调用 OpenGL

不要这样写：

```cpp
void Unit::Render()
{
    glBindTexture(...);
    glDrawElements(...);
}
```

推荐流程：

```text
Game Logic
    ↓
RenderSystem 收集可绘制对象
    ↓
Renderer2D 提交绘制命令
    ↓
OpenGL Backend 执行真正的 glDrawElements
```

也就是说：

```text
Unit / Projectile / Skill 只产生数据
RenderSystem 收集数据
Renderer2D 负责绘制
OpenGL Backend 负责提交
```

这样可以体现工程分层意识。

---

# 5. 推荐目录结构

```text
TinyTactics/
├─ Engine/
│  ├─ Core/
│  │  ├─ Application.h
│  │  ├─ Window.h
│  │  ├─ Layer.h
│  │  ├─ TimeStep.h
│  │  ├─ Input.h
│  │  └─ Log.h
│  │
│  ├─ Renderer/
│  │  ├─ RendererAPI.h
│  │  ├─ Renderer2D.h
│  │  ├─ RenderStats.h
│  │  ├─ Shader.h
│  │  ├─ Texture.h
│  │  ├─ VertexBuffer.h
│  │  ├─ IndexBuffer.h
│  │  ├─ VertexArray.h
│  │  ├─ BufferLayout.h
│  │  ├─ Camera2D.h
│  │  ├─ FrameBuffer.h
│  │  └─ DebugDraw.h
│  │
│  ├─ ECS/
│  │  ├─ Entity.h
│  │  ├─ World.h
│  │  ├─ Components.h
│  │  └─ System.h
│  │
│  ├─ Resource/
│  │  ├─ AssetManager.h
│  │  └─ JsonLoader.h
│  │
│  └─ Debug/
│     ├─ Profiler.h
│     └─ OpenGLDebug.h
│
├─ Game/
│  ├─ Systems/
│  │  ├─ MovementSystem.h
│  │  ├─ CombatSystem.h
│  │  ├─ ProjectileSystem.h
│  │  ├─ PathfindingSystem.h
│  │  ├─ SkillSystem.h
│  │  ├─ BuffSystem.h
│  │  ├─ LifetimeSystem.h
│  │  ├─ CleanupSystem.h
│  │  └─ RenderSystem.h
│  │
│  ├─ Config/
│  │  ├─ units.json
│  │  ├─ skills.json
│  │  └─ map.json
│  │
│  └─ BattleLayer.h
│
├─ Editor/
│  └─ EditorLayer.h
│
├─ Assets/
│  ├─ Textures/
│  └─ Shaders/
│
├─ ThirdParty/
├─ CMakeLists.txt
└─ README.md
```

---

# 6. OpenGL 部分具体要做什么

OpenGL 部分的目标不是“能调用 API 画东西”，而是：

> 理解 OpenGL 从上下文创建、Shader、Buffer、Texture、Camera 到 DrawCall 提交的完整流程，并且能把这些能力封装成一个服务于游戏项目的 2D 渲染系统。

推荐 OpenGL 版本：

```text
OpenGL 3.3 Core Profile
```

原因：

* 足够现代
* 教程和资料多
* 兼容性好
* 不需要一开始碰太新的 OpenGL 特性

推荐依赖：

```text
GLFW：窗口、输入、OpenGL Context
GLAD：加载 OpenGL 函数
glm：数学库
stb_image：图片加载
ImGui：调试面板
nlohmann/json：JSON 配置读取
```

---

# 7. OpenGL 开发路线

建议按下面顺序完成：

```text
1. 创建 OpenGL 上下文
2. 配置基础 OpenGL 状态
3. 画出第一个三角形
4. 封装 Shader
5. 封装 Buffer / VertexArray
6. 画 Quad
7. 加入 Texture
8. 加入 Camera2D
9. 实现 Renderer2D
10. 实现 Batch Renderer
11. 实现 DebugDraw
12. 实现 FrameBuffer
13. 加入 OpenGL Debug / RenderDoc 分析
```

---

# 8. 第一步：创建 OpenGL 上下文

## 8.1 初始化窗口和 Context

需要完成：

```cpp
glfwInit();

glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

GLFWwindow* window = glfwCreateWindow(1280, 720, "TinyTactics", nullptr, nullptr);
glfwMakeContextCurrent(window);

gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

glViewport(0, 0, 1280, 720);
```

处理窗口缩放：

```cpp
glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
});
```

---

## 8.2 基础 OpenGL 状态

2D 游戏至少需要配置透明混合：

```cpp
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

用途：

```text
支持 PNG 透明贴图
支持半透明特效
支持 UI / 血条 / 选中框
```

可以暂时关闭深度测试：

```cpp
glDisable(GL_DEPTH_TEST);
```

2D 渲染排序先用：

```text
sortingOrder
layer
y-sort
```

---

# 9. 第二步：封装 Shader

## 9.1 Shader 类职责

`Shader` 类需要负责：

```text
读取 shader 文件
编译 vertex shader
编译 fragment shader
链接 program
检查编译错误
检查链接错误
设置 uniform
绑定 / 解绑 shader
```

推荐接口：

```cpp
class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat4(const std::string& name, const glm::mat4& value);

private:
    uint32_t rendererID = 0;
};
```

---

## 9.2 基础 2D Sprite Shader

Vertex Shader：

```glsl
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;

    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
```

Fragment Shader：

```glsl
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Textures[16];

void main()
{
    int index = int(v_TexIndex);
    vec4 texColor = texture(u_Textures[index], v_TexCoord);
    color = texColor * v_Color;
}
```

这个 Shader 是为了后面的 Batch Renderer 和多纹理合批做准备。

---

# 10. 第三步：封装 Buffer / VertexArray

OpenGL 的 VAO / VBO / EBO 不要到处裸写。

需要封装：

```text
VertexBuffer
IndexBuffer
VertexArray
VertexBufferLayout
```

推荐目录：

```text
Engine/Renderer/
├─ VertexBuffer.h
├─ IndexBuffer.h
├─ VertexArray.h
└─ BufferLayout.h
```

---

## 10.1 Vertex 格式

2D Sprite 推荐顶点结构：

```cpp
struct QuadVertex {
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texCoord;
    float texIndex;
};
```

对应 layout：

```text
location 0: position, vec3
location 1: color, vec4
location 2: texCoord, vec2
location 3: texIndex, float
```

---

# 11. 第四步：先画一个 Quad

不要一开始直接做复杂系统。

第一阶段目标：

```text
屏幕上画出一个带颜色的矩形
```

Quad 顶点：

```cpp
float vertices[] = {
    // position           // color
    -0.5f, -0.5f, 0.0f,   1, 0, 0, 1,
     0.5f, -0.5f, 0.0f,   0, 1, 0, 1,
     0.5f,  0.5f, 0.0f,   0, 0, 1, 1,
    -0.5f,  0.5f, 0.0f,   1, 1, 1, 1
};
```

Index：

```cpp
uint32_t indices[] = {
    0, 1, 2,
    2, 3, 0
};
```

调用：

```cpp
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
```

这一步证明：

```text
OpenGL Context 正常
Shader 正常
VAO / VBO / EBO 正常
Draw Call 正常
```

---

# 12. 第五步：加入 Texture

## 12.1 Texture2D 类职责

`Texture2D` 需要负责：

```text
使用 stb_image 读取图片
创建 OpenGL texture
设置过滤方式
设置包裹方式
上传像素数据
绑定到指定 slot
释放 texture
```

推荐接口：

```cpp
class Texture2D {
public:
    Texture2D(const std::string& path);
    ~Texture2D();

    void Bind(uint32_t slot = 0) const;

    int GetWidth() const;
    int GetHeight() const;

private:
    uint32_t rendererID = 0;
    int width = 0;
    int height = 0;
    int channels = 0;
};
```

---

## 12.2 图片翻转

stb_image 默认读取出来的图片坐标可能和 OpenGL 纹理坐标方向不一致。

通常需要：

```cpp
stbi_set_flip_vertically_on_load(true);
```

否则图片可能上下颠倒。

---

# 13. 第六步：实现 Camera2D

2D 策略游戏需要摄像机。

Camera2D 需要支持：

```text
位置移动
缩放
窗口宽高变化
世界坐标到屏幕坐标
屏幕坐标到世界坐标
```

核心矩阵：

```cpp
projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
view = glm::inverse(transform);
viewProjection = projection * view;
```

推荐接口：

```cpp
class Camera2D {
public:
    void SetViewportSize(float width, float height);
    void SetPosition(const glm::vec2& position);
    void SetZoom(float zoom);

    const glm::mat4& GetViewProjection() const;

    glm::vec2 ScreenToWorld(const glm::vec2& screenPos) const;
    glm::vec2 WorldToScreen(const glm::vec2& worldPos) const;

private:
    glm::vec2 position = {0.0f, 0.0f};
    float zoom = 1.0f;

    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 viewProjection = glm::mat4(1.0f);
};
```

这一步很重要，因为后续要做：

```text
鼠标点击选中单位
地图格子编辑
拖拽摄像机
缩放地图
攻击范围显示
```

---

# 14. 第七步：实现 Renderer2D

`Renderer2D` 是项目的核心 OpenGL 封装。

游戏层不应该知道 OpenGL 细节，只调用：

```cpp
Renderer2D::BeginScene(camera);
Renderer2D::DrawQuad(position, size, color);
Renderer2D::DrawSprite(position, size, texture);
Renderer2D::EndScene();
```

推荐接口：

```cpp
class Renderer2D {
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const Camera2D& camera);
    static void EndScene();

    static void DrawQuad(const glm::vec2& position,
                         const glm::vec2& size,
                         const glm::vec4& color);

    static void DrawSprite(const glm::vec2& position,
                           const glm::vec2& size,
                           const std::shared_ptr<Texture2D>& texture,
                           const glm::vec4& tintColor = glm::vec4(1.0f));

    static void ResetStats();
    static RendererStats GetStats();
};
```

统计信息：

```cpp
struct RendererStats {
    uint32_t drawCalls = 0;
    uint32_t quadCount = 0;
    uint32_t textureSlotCount = 0;
};
```

---

# 15. 第八步：实现 Batch Renderer

这是 OpenGL 部分最重要的加分点。

## 15.1 为什么要做 Batch

错误做法：

```text
每个地图格子一个 Draw Call
每个单位一个 Draw Call
每个子弹一个 Draw Call
每个特效一个 Draw Call
```

这样 1000 个对象可能就有 1000 次 Draw Call。

正确目标：

```text
把大量 Quad 合并到一个大 VBO 里
一次或少量 glDrawElements 提交
```

README 中可以展示：

```text
Before batching:
Draw Calls: 1200+

After batching:
Draw Calls: 8 ~ 20
```

---

## 15.2 Batch Renderer 数据结构

推荐限制：

```cpp
static const uint32_t MaxQuads = 10000;
static const uint32_t MaxVertices = MaxQuads * 4;
static const uint32_t MaxIndices = MaxQuads * 6;
static const uint32_t MaxTextureSlots = 16;
```

需要维护：

```cpp
struct Renderer2DData {
    std::shared_ptr<VertexArray> quadVertexArray;
    std::shared_ptr<VertexBuffer> quadVertexBuffer;
    std::shared_ptr<Shader> textureShader;

    uint32_t quadIndexCount = 0;

    QuadVertex* quadVertexBufferBase = nullptr;
    QuadVertex* quadVertexBufferPtr = nullptr;

    std::array<std::shared_ptr<Texture2D>, MaxTextureSlots> textureSlots;
    uint32_t textureSlotIndex = 1;

    RendererStats stats;
};
```

---

## 15.3 Batch 工作流程

```text
BeginScene
    清空当前批次数据

DrawSprite / DrawQuad
    把 Quad 的 4 个顶点写入 CPU 侧缓存
    累加 index count
    检查 texture slot
    如果满了就 Flush

EndScene
    上传整批顶点数据到 GPU
    绑定纹理
    glDrawElements 一次绘制
```

伪代码：

```cpp
void Renderer2D::BeginScene(const Camera2D& camera)
{
    s_Data.textureShader->Bind();
    s_Data.textureShader->SetMat4("u_ViewProjection", camera.GetViewProjection());

    StartBatch();
}

void Renderer2D::EndScene()
{
    Flush();
}

void Renderer2D::Flush()
{
    if (s_Data.quadIndexCount == 0)
        return;

    uint32_t dataSize =
        (uint8_t*)s_Data.quadVertexBufferPtr -
        (uint8_t*)s_Data.quadVertexBufferBase;

    s_Data.quadVertexBuffer->SetData(s_Data.quadVertexBufferBase, dataSize);

    for (uint32_t i = 0; i < s_Data.textureSlotIndex; i++)
        s_Data.textureSlots[i]->Bind(i);

    s_Data.quadVertexArray->Bind();
    glDrawElements(GL_TRIANGLES, s_Data.quadIndexCount, GL_UNSIGNED_INT, nullptr);

    s_Data.stats.drawCalls++;
}
```

---

## 15.4 Texture Slot 处理

因为一个 batch 里可能有多个纹理，所以需要 texture slot。

逻辑：

```text
如果当前纹理已经在 textureSlots 中，复用对应 texIndex
如果不在，加入新的 texture slot
如果 texture slot 满了，Flush 当前 batch，然后开启新 batch
```

这可以证明你理解：

```text
OpenGL texture binding
sampler2D array
GPU 一次 draw 中如何区分不同贴图
```

---

# 16. 第九步：DebugDraw

DebugDraw 用于绘制调试信息。

需要支持：

```text
DrawLine
DrawRect
DrawCircle
DrawGrid
DrawPath
DrawAttackRange
DrawCollider
```

推荐接口：

```cpp
class DebugDraw {
public:
    static void DrawLine(const glm::vec2& start,
                         const glm::vec2& end,
                         const glm::vec4& color);

    static void DrawRect(const glm::vec2& center,
                         const glm::vec2& size,
                         const glm::vec4& color);

    static void DrawCircle(const glm::vec2& center,
                           float radius,
                           const glm::vec4& color);
};
```

用途：

```text
显示 A* 路径
显示单位攻击范围
显示选中框
显示碰撞盒
显示地图格子
```

这部分非常实用，能证明你会做开发工具。

---

# 17. 第十步：FrameBuffer

FrameBuffer 是进阶加分项，不是第一天必须做。

可以用于：

```text
游戏画面渲染到 ImGui 窗口中
后处理效果
屏幕截图
小地图
选中描边
RenderTexture
```

推荐封装：

```cpp
class FrameBuffer {
public:
    FrameBuffer(uint32_t width, uint32_t height);
    ~FrameBuffer();

    void Bind();
    void Unbind();

    void Resize(uint32_t width, uint32_t height);

    uint32_t GetColorAttachmentRendererID() const;

private:
    uint32_t rendererID = 0;
    uint32_t colorAttachment = 0;
    uint32_t depthAttachment = 0;
};
```

项目里可以用它实现：

```text
Editor 窗口中显示 Game View
```

也就是：

```text
OpenGL 渲染到 FrameBuffer
ImGui 显示 FrameBuffer 的 color texture
```

这很像真实编辑器的工作方式。

---

# 18. 第十一步：OpenGL Debug 能力

项目中建议加入 OpenGL Debug。

## 18.1 glGetError

可以先写一个简单宏：

```cpp
#define GLCall(x) \
    GLClearError(); \
    x; \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
```

用于定位 OpenGL 调用错误。

---

## 18.2 Debug Callback

如果环境支持，可以启用：

```cpp
glEnable(GL_DEBUG_OUTPUT);
glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
glDebugMessageCallback(OpenGLMessageCallback, nullptr);
```

这样 OpenGL 错误可以直接输出到日志。

这可以体现你不仅会写渲染，也知道怎么调试渲染问题。

---

## 18.3 RenderDoc

推荐在 README 里展示一张 RenderDoc 截帧图，说明：

```text
当前场景 Draw Call 数量
Batch 是否生效
Texture 是否正确绑定
Vertex Buffer 是否正确上传
```

这对面试有加分效果。

---

# 19. OpenGL 部分验收标准

## 19.1 V1 验收

完成后应该具备：

```text
可以打开窗口
可以清屏
可以画彩色 Quad
可以画 Texture Quad
可以移动 Camera2D
可以把鼠标屏幕坐标转换到世界坐标
可以显示 ImGui
可以显示 FPS 和 DrawCall
```

---

## 19.2 V2 验收

完成后应该具备：

```text
可以批量绘制地图格子
可以批量绘制单位
可以批量绘制子弹
可以绘制血条
可以绘制选中框
可以绘制攻击范围
可以绘制寻路路径
Renderer2D 有 DrawCall / QuadCount 统计
```

---

## 19.3 V3 验收

完成后应该具备：

```text
实现真正的 2D Batch Renderer
支持多个 Texture Slot
支持 FrameBuffer
支持 DebugDraw
支持 OpenGL Debug Callback
可以用 RenderDoc 截帧分析
README 中展示 DrawCall 优化前后对比
```

---

# 20. ECS-like 设计

## 20.1 Entity

Entity 只作为 ID，不负责逻辑。

```cpp
using EntityID = uint32_t;

constexpr EntityID InvalidEntity = 0;
```

---

## 20.2 World

World 负责管理所有实体和组件。

```cpp
struct World {
    EntityID nextEntity = 1;

    std::vector<EntityID> aliveEntities;

    std::unordered_map<EntityID, TransformComponent> transforms;
    std::unordered_map<EntityID, SpriteComponent> sprites;
    std::unordered_map<EntityID, VelocityComponent> velocities;
    std::unordered_map<EntityID, UnitStatsComponent> units;
    std::unordered_map<EntityID, AttackComponent> attacks;
    std::unordered_map<EntityID, ProjectileComponent> projectiles;
    std::unordered_map<EntityID, PathComponent> paths;
    std::unordered_map<EntityID, GridAgentComponent> gridAgents;
};
```

---

## 20.3 基础组件

```cpp
struct TransformComponent {
    glm::vec2 position = {0.0f, 0.0f};
    float rotation = 0.0f;
    glm::vec2 scale = {1.0f, 1.0f};
};
```

```cpp
struct SpriteComponent {
    TextureID texture;
    glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    int sortingOrder = 0;
};
```

```cpp
struct VelocityComponent {
    glm::vec2 velocity = {0.0f, 0.0f};
    float moveSpeed = 1.0f;
};
```

---

## 20.4 战斗组件

```cpp
struct UnitStatsComponent {
    int maxHp = 100;
    int currentHp = 100;
    int attack = 10;
    float attackRange = 3.0f;
    int team = 0;
};
```

```cpp
struct AttackComponent {
    float cooldown = 1.0f;
    float timer = 0.0f;
};
```

```cpp
struct ProjectileComponent {
    EntityID owner = InvalidEntity;
    EntityID target = InvalidEntity;
    int damage = 10;
    float speed = 5.0f;
};
```

```cpp
struct BuffComponent {
    std::string buffId;
    float duration = 0.0f;
    float timer = 0.0f;
};
```

---

## 20.5 寻路组件

```cpp
struct PathComponent {
    std::vector<glm::ivec2> path;
    int currentIndex = 0;
};
```

```cpp
struct GridAgentComponent {
    glm::ivec2 currentCell = {0, 0};
    glm::ivec2 targetCell = {0, 0};
    bool needRepath = false;
};
```

---

# 21. 系统设计

## 21.1 MovementSystem

职责：

* 根据 Velocity 更新 Transform
* 根据 PathComponent 移动单位
* 处理单位移动速度

输入组件：

```text
TransformComponent
VelocityComponent
PathComponent
GridAgentComponent
```

---

## 21.2 CombatSystem

职责：

* 寻找最近敌人
* 判断攻击范围
* 处理攻击冷却
* 创建子弹或直接造成伤害

输入组件：

```text
TransformComponent
UnitStatsComponent
AttackComponent
```

---

## 21.3 ProjectileSystem

职责：

* 更新子弹位置
* 判断是否命中目标
* 命中后造成伤害
* 回收子弹实体

输入组件：

```text
TransformComponent
ProjectileComponent
```

---

## 21.4 PathfindingSystem

职责：

* 基于网格地图执行 A* 寻路
* 根据障碍物刷新路径
* 生成 PathComponent

---

## 21.5 SkillSystem

职责：

* 技能释放
* AOE 判定
* 技能冷却
* 技能效果执行

---

## 21.6 BuffSystem

职责：

* Buff 持续时间更新
* Buff 效果应用
* Buff 结束后移除效果

---

## 21.7 RenderSystem

职责：

* 根据 Transform + Sprite 绘制实体
* 绘制血条
* 绘制攻击范围
* 绘制选中框
* 绘制 Debug 信息

注意：

```text
RenderSystem 可以调用 Renderer2D
但普通游戏逻辑系统不要直接调用 OpenGL
```

---

# 22. 主循环顺序

建议主循环采用 Fixed Timestep。

逻辑更新顺序：

```cpp
void BattleLayer::OnUpdate(float dt)
{
    inputSystem.Update(world, dt);

    pathfindingSystem.Update(world, dt);
    movementSystem.Update(world, dt);

    combatSystem.Update(world, dt);
    projectileSystem.Update(world, dt);
    skillSystem.Update(world, dt);
    buffSystem.Update(world, dt);

    lifetimeSystem.Update(world, dt);
    cleanupSystem.Update(world, dt);

    renderSystem.Render(world);
}
```

推荐设计：

```text
Update: fixed 60 tick/s
Render: variable frame rate
```

这样可以保证战斗逻辑稳定，便于之后做回放系统。

---

# 23. 数据驱动配置

不要把单位和技能数据全部写死在 C++ 里。

## 23.1 单位配置示例

```json
{
  "id": "archer",
  "hp": 80,
  "attack": 12,
  "range": 5.0,
  "moveSpeed": 2.5,
  "attackInterval": 0.8,
  "texture": "archer.png"
}
```

---

## 23.2 技能配置示例

```json
{
  "id": "fireball",
  "damage": 30,
  "radius": 2.5,
  "cooldown": 4.0,
  "projectileSpeed": 6.0,
  "effect": "aoe_damage"
}
```

---

## 23.3 地图配置示例

```json
{
  "width": 20,
  "height": 12,
  "blockedCells": [
    [3, 4],
    [3, 5],
    [4, 5]
  ],
  "spawnPoints": [
    [0, 5]
  ],
  "basePoint": [19, 5]
}
```

---

# 24. Debug / Editor 工具

使用 ImGui 做调试面板。

建议功能：

```text
FPS 显示
Frame Time 显示
Update Time 显示
Render Time 显示
Draw Call 数量
Quad Count
Texture Slot Count
Entity 数量
Projectile 数量
Pathfinding 耗时
当前选中单位属性
修改单位 HP / Attack / Speed
显示攻击范围
显示寻路路径
显示碰撞盒
地图格子编辑
障碍物放置
地图保存 / 读取
```

这部分能证明不仅会写功能，也会做调试工具。

---

# 25. 性能优化点

## 25.1 对象池

用于：

```text
Projectile
Floating Damage Text
Temporary Effect
```

示例目标：

```text
Projectile system uses object pool to reduce runtime allocation.
```

---

## 25.2 Draw Call 统计

Renderer2D 需要统计：

```text
Draw Call 数量
Quad 数量
Texture Slot 使用数量
Texture 切换次数
```

---

## 25.3 Profiler

Profiler 显示：

```text
FPS
Frame Time
Update Time
Render Time
Draw Calls
Quad Count
Entity Count
Projectile Count
Pathfinding Cost
```

---

# 26. 版本规划

## 26.1 V1：基础可运行版

目标：先让项目跑起来。

功能：

```text
CMake 工程
GLFW + GLAD + OpenGL
基础窗口
OpenGL Debug 基础封装
Shader / Texture / Buffer 基础封装
Camera2D
Renderer2D
绘制地图格子
绘制单位
鼠标点击选中单位
ImGui 面板
FPS / DrawCall 显示
```

---

## 26.2 V2：可展示版

目标：可以放 GitHub 和简历。

功能：

```text
A* 寻路
单位移动
自动寻敌
自动攻击
子弹系统
血条
单位死亡
JSON 单位配置
JSON 地图配置
DebugDraw
简单地图编辑器
批量绘制地图、单位、子弹
```

---

## 26.3 V3：面试加分版

目标：面试时可以深入讲技术。

功能：

```text
2D Batch Renderer
多个 Texture Slot
对象池
Fixed Timestep
战斗回放雏形
性能 Profiler
FrameBuffer Game View
场景序列化
单元测试
GitHub Actions 自动构建
README 架构图
演示 GIF / 视频
RenderDoc 截帧分析
DrawCall 优化前后对比
```

---

# 27. README 结构

README 应该像作品介绍，而不是普通代码仓库。

推荐结构：

```md
# TinyTactics Engine

A small OpenGL/C++ tactical battle sandbox focused on rendering, gameplay systems, debugging tools, and performance optimization.

## Features

- OpenGL 2D batch renderer
- Lightweight ECS-like architecture
- Data-driven unit and skill config
- A* pathfinding on grid map
- Projectile and buff system
- ImGui-based debug/editor panel
- Fixed timestep game loop
- Runtime profiler and debug draw
- Object pool for projectiles and effects
- FrameBuffer-based editor game view

## Screenshots / GIFs

## Architecture

- Engine Layer
- Game Layer
- Editor Layer

## OpenGL Rendering Pipeline

- Shader
- Texture
- VertexBuffer / IndexBuffer / VertexArray
- Camera2D
- Renderer2D
- Batch Renderer
- DebugDraw
- FrameBuffer

## Technical Highlights

1. Renderer2D batching
2. Data-driven battle system
3. Fixed timestep simulation
4. Debug and profiling tools
5. Object pool for runtime allocation reduction
6. FrameBuffer-based editor view

## Performance

Before batching:
Draw Calls: 1200+

After batching:
Draw Calls: 8 ~ 20

## Build

- C++17
- CMake
- OpenGL 3.3 Core Profile
- GLFW
- GLAD
- ImGui
- glm
- stb_image
- nlohmann/json

## What I Learned

Explain engineering tradeoffs and implementation details.
```

---

# 28. 简历描述

## 28.1 完整版本

> 使用 C++17 与 OpenGL 开发小型 2D 策略战斗引擎 Demo，完成渲染层、资源管理、场景系统、输入系统、ImGui 调试工具与数据驱动战斗系统。封装 Shader、Texture、VertexBuffer、IndexBuffer、VertexArray、Camera2D、FrameBuffer 等 OpenGL 模块，实现 Sprite Batch Renderer、A* 寻路、对象池、固定时间步长主循环、JSON 配置加载、运行时性能统计等功能，并通过 Draw Call 统计与对象复用优化运行性能。

---

## 28.2 简短版本

> 基于 C++17 / OpenGL 实现小型策略战斗引擎 Demo，包含 2D 批渲染、轻量 ECS-like 架构、A* 寻路、数据驱动技能系统、ImGui 编辑器、对象池和性能 Profiler，展示游戏客户端工程架构与渲染优化能力。

---

## 28.3 偏 OpenGL 版本

> 封装 OpenGL 渲染层，实现 Shader、Texture、VertexBuffer、IndexBuffer、VertexArray、Camera2D、FrameBuffer 等模块，并基于动态顶点缓冲实现 2D Sprite Batch Renderer，支持多纹理合批、DrawCall 统计、DebugDraw 和 ImGui 性能面板。

---

## 28.4 偏性能优化版本

> 实现 OpenGL 2D Batch Renderer，将地图块、单位、子弹和特效统一写入动态顶点缓冲，通过 Texture Slot 和 sampler2D array 支持多纹理合批，减少大量 Sprite 场景下的 DrawCall 数量，并在 ImGui 面板中实时显示 DrawCall、QuadCount 和 FrameTime。

---

# 29. 面试讲法

## 29.1 为什么做 Engine / Game 分层？

可以回答：

> 我希望把窗口、渲染、资源、输入、时间管理封装到 Engine 层，Game 层只关注战斗逻辑。这样后续可以替换不同玩法 Demo，同时复用底层渲染和调试工具。

---

## 29.2 为什么使用轻量 ECS-like 架构？

可以回答：

> 这个项目采用轻量 ECS-like 架构。Entity 本身只是一个 ID，真正的数据保存在 Component 中，例如 Transform、Sprite、UnitStats、Attack、Projectile。System 负责遍历拥有特定组件组合的实体并执行逻辑，例如 MovementSystem 处理 Transform 和 Velocity，CombatSystem 处理 UnitStats 和 Attack。这样可以避免传统继承结构在单位、子弹、技能、Buff 扩展时出现继承爆炸，也方便后续做编辑器、序列化和性能分析。

---

## 29.3 为什么不直接使用完整 ECS 或 entt？

可以回答：

> 我没有一开始引入完整 ECS 框架，而是根据 Demo 规模实现了轻量 ECS-like 架构。这样既能保持数据和逻辑分离，也避免过早工程复杂化。第一版重点是让项目完整跑起来，后续如果实体数量和系统复杂度上升，可以再迁移到 entt 或 Sparse Set 结构。

---

## 29.4 为什么做 Batch Renderer？

可以回答：

> 俯视角策略游戏会有大量地图块、单位、子弹和特效。如果每个对象单独提交 Draw Call，CPU 侧开销会很高。所以我把相同顶点格式的 Sprite 合并到动态顶点缓冲中，通过 Texture Slot 和 sampler2D array 支持多纹理合批，减少 Draw Call 数量，并在 ImGui 面板中显示优化前后的 Draw Call 变化。

---

## 29.5 为什么用 Fixed Timestep？

可以回答：

> 战斗逻辑需要稳定，尤其是攻击间隔、寻路、子弹移动和回放系统。如果 Update 完全依赖渲染帧率，不同机器上结果可能不同。所以逻辑层使用固定时间步，渲染层按当前状态绘制或插值。

---

## 29.6 为什么做 JSON 配置？

可以回答：

> 游戏项目里数值和逻辑需要分离。单位、技能、Buff、地图数据放在 JSON 中，程序负责加载和运行，这样可以模拟实际项目里程序和策划协作的工作方式。

---

## 29.7 OpenGL 部分你具体做了什么？

可以回答：

> 我从 OpenGL 3.3 Core Profile 上下文开始搭建，封装了 Shader、Texture、VertexBuffer、IndexBuffer、VertexArray、Camera2D 和 FrameBuffer。然后在这些基础上实现 Renderer2D，对外提供 DrawQuad 和 DrawSprite 接口。后续为了减少大量 Sprite 场景下的 Draw Call，我实现了 2D Batch Renderer，把多个 Quad 写入动态顶点缓冲，并用 Texture Slot 支持多纹理合批。

---

# 30. 不建议做的方向

## 30.1 不建议一开始做纯 3D 渲染器

例如：

```text
PBR
IBL
SSAO
延迟渲染
阴影贴图
骨骼动画
```

这些适合渲染岗，但不一定最适合游戏客户端求职项目。

如果目标是游戏客户端，推荐做：

```text
OpenGL 渲染能力 + 游戏系统能力 + 工程架构能力 + 调试工具能力
```

---

## 30.2 不建议做过大的开放世界项目

不要做：

```text
完整 3D RPG
类 Unity 编辑器
完整 RTS
大型开放世界
```

这些容易烂尾。

求职项目重点是：

```text
小而完整
能运行
能展示
能解释
能优化
```

---

# 31. OpenGL 部分常见坑

## 31.1 只封装了类，但没有证明效果

不能只写：

```text
我封装了 Shader / Texture / Buffer
```

还要展示：

```text
这些封装如何支持游戏系统
Batch 前后 DrawCall 变化
DebugDraw 如何帮助寻路调试
FrameBuffer 如何支持编辑器 Game View
```

---

## 31.2 游戏逻辑直接调用 OpenGL

不要让 Unit、Projectile、Skill 直接调用 `glDrawElements`。

应该是：

```text
Unit / Projectile / Skill 只产生数据
RenderSystem 收集数据
Renderer2D 负责绘制
OpenGL Backend 负责提交
```

---

## 31.3 一开始做太复杂的 3D

不要一开始做：

```text
PBR
SSAO
延迟渲染
骨骼动画
阴影贴图
```

这个项目重点是游戏客户端综合能力，不是纯渲染岗作品。

更适合先做：

```text
2D Camera
Sprite Renderer
Batch Renderer
DebugDraw
FrameBuffer
Profiler
```

---

## 31.4 没有统计数据

一定要有统计：

```text
Draw Calls
Quad Count
Texture Slot Count
Frame Time
Render Time
```

这些数据是证明你做了优化的关键。

---

# 32. Codex 开发优先级

让 Codex 辅助时，建议按以下顺序生成代码：

```text
1. 创建 CMake 工程，引入 GLFW、GLAD、glm、stb_image、ImGui、nlohmann/json
2. 实现 Application 和 Window，创建 OpenGL 3.3 Core Profile 上下文
3. 加入 OpenGL 错误检查和 Debug Callback
4. 实现 Shader 类，支持从文件加载、编译、链接和设置 uniform
5. 实现 VertexBuffer、IndexBuffer、VertexArray、BufferLayout
6. 实现 Texture2D，使用 stb_image 加载 PNG
7. 实现 Camera2D，支持移动、缩放、屏幕坐标和世界坐标转换
8. 实现 Renderer2D，先支持 DrawQuad 和 DrawSprite
9. 实现 RendererStats，统计 DrawCall、QuadCount、TextureSlotCount
10. 加入 ImGui，显示 FPS、FrameTime、DrawCall
11. 实现 World / EntityID / Components
12. 实现地图格子渲染
13. 实现单位创建和选中
14. 实现 MovementSystem
15. 实现 A* PathfindingSystem
16. 实现 CombatSystem
17. 实现 ProjectileSystem
18. 实现 JSON 配置加载
19. 实现 Batch Renderer，使用动态 VBO 批量提交 Quad
20. 实现 Texture Slot，支持多个纹理合批
21. 实现 DebugDraw，支持线段、矩形、圆形、路径和网格
22. 实现 FrameBuffer，把游戏画面渲染到 ImGui Game View
23. 实现对象池
24. 实现 Profiler 和 Debug 面板
25. 实现简单地图编辑器
26. 整理 README、截图、GIF、架构图和 DrawCall 优化对比
```

---

# 33. 最终展示重点

项目完成后，面试时要重点展示：

```text
1. C++ 工程结构
2. Engine / Game / Editor 分层
3. OpenGL 初始化和状态管理
4. Shader / Texture / Buffer 封装
5. Camera2D 和坐标转换
6. Renderer2D 接口设计
7. Sprite Batch Renderer
8. Texture Slot 多纹理合批
9. DebugDraw 调试绘制
10. FrameBuffer 编辑器视图
11. ECS-like 架构
12. 游戏系统更新顺序
13. JSON 数据驱动
14. DrawCall / QuadCount / FrameTime 统计
15. 对象池和运行时分配优化
16. RenderDoc 或 OpenGL Debug Callback 调试能力
17. README 和演示视频
```

---

# 34. 最终项目目标总结

最终这个项目要表达的是：

> 我理解 OpenGL 从上下文创建、Shader、Buffer、Texture、Camera 到 DrawCall 提交的完整流程，并且能把这些能力封装成一个服务于游戏项目的 2D 渲染系统。

同时也要表达：

> 我不只是会写渲染 Demo，而是能把 OpenGL 渲染、C++ 工程架构、游戏系统、调试工具、数据配置和性能优化整合成一个完整可运行的小型游戏项目。

这个方向最适合作为游戏客户端 / C++ / OpenGL 求职项目。
