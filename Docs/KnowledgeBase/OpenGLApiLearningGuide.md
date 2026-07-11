# OpenGL API 学习笔记

## 1. 这个文档记录什么

这个文档集中记录 TinyTactics 当前阶段用到的 OpenGL / GLAD / GLSL 相关 API。

原则：

- 只记录项目里已经用到或马上要用到的 API。
- 先解释“它在当前项目里负责什么”，再解释 OpenGL 层面的含义。
- Shader 内部细节可以先当黑箱，但 OpenGL API 名字和调用位置要集中记录在这里。

## 2. OpenGL、GLAD、GLSL、GLFW 的关系

当前项目里这几个名字经常一起出现，但它们不是同一种东西。

```text
GLFW:
    负责创建窗口、创建 OpenGL Context、处理输入和窗口事件。

GLAD:
    负责加载 OpenGL 函数指针，让 C++ 代码能调用 glXXX 函数。

OpenGL:
    图形 API，本体是一套状态机式的渲染接口，例如 glClear、glDrawElements。

GLSL:
    OpenGL 使用的 shader 语言，用来写 GPU 上执行的小程序。
```

在 TinyTactics 里的启动顺序可以理解为：

```text
Window::Initialize()
    -> glfwInit()
    -> glfwCreateWindow()
    -> glfwMakeContextCurrent()
    -> gladLoadGL()
    -> glViewport()
    -> RenderCommand::Init()
```

其中：

```text
glfwCreateWindow:
    创建窗口。

glfwMakeContextCurrent:
    让当前线程拥有一个可用的 OpenGL Context。

gladLoadGL:
    在 OpenGL Context 已经存在后，加载 glXXX 函数。

glViewport / glEnable / glClear:
    才是真正的 OpenGL API 调用。
```

如果没有 GLFW 创建出来的 OpenGL Context，GLAD 没有东西可加载。  
如果没有 GLAD，C++ 代码不能可靠调用现代 OpenGL 函数。  
如果没有 GLSL shader，OpenGL Core Profile 不能正常绘制自定义图形。

## 3. 当前阶段最常见的 OpenGL 对象

OpenGL 里经常不是“直接画”，而是先创建对象，再绑定对象，再发起绘制。

### Context

`OpenGL Context` 是 OpenGL 的运行环境。可以理解为：

```text
当前线程里 OpenGL 的全部状态集合。
```

例如当前绑定了哪个 shader、哪个 VAO、哪个 framebuffer，都属于 Context 里的状态。

当前项目里 Context 由 GLFW 创建：

```cpp
glfwCreateWindow(...);
glfwMakeContextCurrent(m_Handle);
```

### Shader Object

Shader Object 是单个 shader 阶段的编译对象，例如：

```text
vertex shader
fragment shader
```

相关 API：

```text
glCreateShader
glShaderSource
glCompileShader
glGetShaderiv
glGetShaderInfoLog
glDeleteShader
```

它的生命周期是：

```text
创建 shader object
    -> 填入 GLSL 源码
    -> 编译
    -> 附加到 program
    -> program 链接成功后可以删除 shader object
```

### Shader Program

Shader Program 是最终绘制时绑定的 GPU 程序。它通常由多个 shader stage 链接而成：

```text
vertex shader + fragment shader -> shader program
```

相关 API：

```text
glCreateProgram
glAttachShader
glLinkProgram
glUseProgram
glDeleteProgram
```

项目里的 `TinyTactics::Shader` 类主要包装的就是 Shader Program。

### Buffer Object

Buffer Object 是 GPU 侧的一段数据存储。

当前项目里主要有两种：

```text
VertexBuffer:
    顶点数据，例如 position、color、texCoord。

IndexBuffer:
    索引数据，告诉 OpenGL 哪几个顶点组成三角形。
```

相关 API：

```text
glGenBuffers
glBindBuffer
glBufferData
glDeleteBuffers
```

### Vertex Array Object

VAO 不是顶点数据本身，而是“顶点数据读取规则”的记录。

它记录：

```text
哪些 attribute location 被启用
每个 attribute 从 VBO 的哪个 offset 开始读
每个 attribute 有几个 float
每个顶点 stride 是多少
当前关联的 index buffer
```

相关 API：

```text
glGenVertexArrays
glBindVertexArray
glEnableVertexAttribArray
glVertexAttribPointer
glDeleteVertexArrays
```

项目里的 `BufferLayout` 最后会落到：

```cpp
glVertexAttribPointer(...)
```

### Uniform

Uniform 是从 C++ 传给 shader program 的全局参数。

常见用途：

```text
相机矩阵
颜色
材质参数
贴图槽位
时间
```

当前摄像头功能里的：

```glsl
uniform mat4 u_ViewProjection;
```

就是一个 uniform。

相关 API：

```text
glGetUniformLocation
glUniformMatrix4fv
glUniform4fv
```

## 4. 常见调用类型

OpenGL API 可以先粗略分成几类，不需要按字母记。

### 初始化类

```text
gladLoadGL
glViewport
```

初始化类 API 负责让 OpenGL 可用，并告诉 OpenGL 当前画布尺寸。

### 状态类

```text
glEnable
glDisable
glBlendFunc
glUseProgram
glBindBuffer
glBindVertexArray
```

OpenGL 是状态机。状态类 API 的含义通常是：

```text
把当前 OpenGL Context 的某个状态改掉。
后面的 draw call 会使用这些当前状态。
```

### 资源创建/销毁类

```text
glGenBuffers
glDeleteBuffers
glGenVertexArrays
glDeleteVertexArrays
glCreateShader
glDeleteShader
glCreateProgram
glDeleteProgram
```

这类 API 负责创建或释放 OpenGL 资源。  
在 C++ 封装里通常对应构造函数、析构函数或 `Release()`。

### 数据上传类

```text
glBufferData
glShaderSource
glUniformMatrix4fv
glUniform4fv
```

这类 API 负责把 CPU 侧的数据传给 OpenGL / GPU。

### 描述数据格式类

```text
glEnableVertexAttribArray
glVertexAttribPointer
```

这类 API 不上传顶点数据，而是告诉 OpenGL：

```text
已经上传的顶点数据应该怎么解释。
```

例如：

```text
location 0 是 position，读 3 个 float。
location 1 是 color，读 4 个 float。
```

### 查询/错误日志类

```text
glGetShaderiv
glGetShaderInfoLog
glGetProgramiv
glGetProgramInfoLog
glGetUniformLocation
```

这类 API 负责查询 OpenGL 对象状态、错误信息或变量位置。

### 执行类

```text
glClear
glDrawElements
```

这类 API 会真正触发操作：

```text
glClear:
    清屏。

glDrawElements:
    发起绘制。
```

## 5. 当前项目的最小绘制链路

以一个彩色矩形为例，链路是：

```text
启动阶段:
    GLFW 创建窗口和 OpenGL Context
    GLAD 加载 OpenGL 函数
    RenderCommand 初始化 OpenGL 状态

资源阶段:
    Shader 读取 GLSL 文件
    OpenGL 编译 shader
    OpenGL 链接 shader program
    VertexBuffer 上传顶点数据
    IndexBuffer 上传索引数据
    VertexArray 记录顶点布局

每帧阶段:
    RenderCommand::Clear()
    Shader::Bind()
    Shader::SetMat4("u_ViewProjection", ...)
    VertexArray::Bind()
    RenderCommand::DrawIndexed()
    Window::OnUpdate()
```

对应到 OpenGL / GLFW API：

```text
glfwCreateWindow
glfwMakeContextCurrent
gladLoadGL

glCreateShader
glCompileShader
glCreateProgram
glLinkProgram

glGenBuffers
glBufferData
glGenVertexArrays
glVertexAttribPointer

glClear
glUseProgram
glUniformMatrix4fv
glDrawElements

glfwSwapBuffers
glfwPollEvents
```

其中 `glfwXXX` 是 GLFW，不是 OpenGL；`glXXX` 是 OpenGL / GLAD 加载出来的 OpenGL 函数。

## 6. 和 C++ 封装类的对应关系

当前项目建议这样记：

```text
Window:
    GLFW window + OpenGL Context + GLAD 初始化 + glViewport

RenderCommand:
    glClearColor / glClear / glEnable / glDrawElements

Shader:
    glCreateShader / glCompileShader / glCreateProgram / glUseProgram / glUniform*

VertexBuffer:
    glGenBuffers / glBindBuffer / glBufferData / glDeleteBuffers

IndexBuffer:
    glGenBuffers / glBindBuffer / glBufferData / glDeleteBuffers

VertexArray:
    glGenVertexArrays / glBindVertexArray / glEnableVertexAttribArray / glVertexAttribPointer

Camera2D:
    不直接调用 OpenGL，负责算矩阵。
    算出来的矩阵通过 Shader::SetMat4 上传给 OpenGL。
```

## 7. 摄像头功能里涉及的 OpenGL 相关 API

`Camera2D` 本身通常不直接调用 OpenGL API。它主要用 `glm` 在 CPU 侧计算矩阵：

```text
position / zoom / viewport
    -> Camera2D::Recalculate()
    -> m_ViewProjection
```

真正和 OpenGL 发生关系的是 shader uniform 上传和 viewport 设置。

### `glViewport`

项目位置：

```text
src/Engine/Core/Window.cpp
```

典型调用：

```cpp
glViewport(0, 0, width, height);
```

作用：

```text
告诉 OpenGL 最终把裁剪空间 / NDC 映射到窗口里的哪一块像素区域。
```

在 2D 摄像头里，`Camera2D::SetViewportSize(width, height)` 负责让相机知道窗口比例；`glViewport(width, height)` 负责让 OpenGL 知道实际绘制到窗口的像素范围。

两者不是一回事：

```text
Camera2D viewport size:
    用来计算投影矩阵，避免画面比例错误。

glViewport:
    用来告诉 OpenGL 绘制区域在窗口哪里、多大。
```

如果窗口 resize 后只改 `glViewport`，但不改 `Camera2D` 的 viewport size，画面比例可能仍然不符合预期。

### `glUseProgram`

项目位置：

```text
src/Engine/Renderer/Shader.cpp
```

典型调用：

```cpp
glUseProgram(m_RendererID);
glUseProgram(0);
```

作用：

```text
选择当前后续 draw call 使用哪个 shader program。
```

在摄像头功能里，传 `u_ViewProjection` 之前需要先绑定对应 shader：

```cpp
shader.Bind();
shader.SetMat4("u_ViewProjection", camera.GetViewProjection());
```

因为 `glUniform*` 修改的是当前绑定的 program。

### `glGetUniformLocation`

项目位置：

```text
src/Engine/Renderer/Shader.cpp
```

典型调用：

```cpp
glGetUniformLocation(m_RendererID, "u_ViewProjection");
```

作用：

```text
查找 shader program 里某个 uniform 变量的位置。
```

`u_ViewProjection` 在 GLSL 里声明：

```glsl
uniform mat4 u_ViewProjection;
```

C++ 侧要先通过名字找到它的位置，才能上传矩阵。

注意：

```text
如果 uniform 没有被 shader 实际使用，OpenGL 可能会优化掉它。
这时 glGetUniformLocation 可能返回 -1。
```

### `glUniformMatrix4fv`

项目位置：

```text
src/Engine/Renderer/Shader.cpp
```

典型调用：

```cpp
glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
```

作用：

```text
把 C++ 侧的 4x4 矩阵上传到当前绑定的 shader program。
```

在摄像头功能里，它负责把：

```cpp
camera.GetViewProjection()
```

传给 shader 里的：

```glsl
uniform mat4 u_ViewProjection;
```

参数含义：

```text
location:
    uniform 变量位置。

1:
    上传 1 个 mat4。

GL_FALSE:
    不转置矩阵。GLM 的默认内存布局适合这样传给 OpenGL。

glm::value_ptr(matrix):
    取得矩阵底层 float 指针。
```

### `gl_Position`

`gl_Position` 不是 C++ OpenGL 函数，而是 GLSL vertex shader 的内置输出变量。

典型代码：

```glsl
gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
```

作用：

```text
vertex shader 必须输出 gl_Position。
OpenGL 后续根据它做裁剪、透视除法、viewport 映射和光栅化。
```

没有相机矩阵时：

```glsl
gl_Position = vec4(a_Position, 1.0);
```

这表示 `a_Position` 已经是裁剪空间坐标。

有相机矩阵时：

```glsl
gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
```

这表示 `a_Position` 是世界坐标，由相机矩阵转换到 OpenGL 需要的坐标空间。

## 3. 当前项目已经用到的 OpenGL API 总表

### GLAD 初始化

#### `gladLoadGL`

项目位置：

```text
src/Engine/Core/Window.cpp
```

作用：

```text
加载当前 OpenGL Context 下可用的 OpenGL 函数指针。
```

GLFW 创建 OpenGL Context 后，GLAD 才能加载函数。

调用顺序是：

```text
glfwCreateWindow
glfwMakeContextCurrent
gladLoadGL
```

### 基础渲染状态

#### `glEnable`

项目位置：

```text
src/Engine/Renderer/RenderCommand.cpp
```

当前用途：

```cpp
glEnable(GL_BLEND);
```

作用：

```text
开启某个 OpenGL 功能开关。
```

这里开启的是 alpha blending，让透明颜色可以正确混合。

#### `glBlendFunc`

当前用途：

```cpp
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

作用：

```text
设置透明混合公式。
```

这是 2D 渲染最常见的透明混合配置。

#### `glDisable`

当前用途：

```cpp
glDisable(GL_DEPTH_TEST);
```

作用：

```text
关闭某个 OpenGL 功能开关。
```

当前是 2D 渲染，所以先关闭深度测试。

### 清屏

#### `glClearColor`

当前用途：

```cpp
glClearColor(red, green, blue, alpha);
```

作用：

```text
设置下一次清空颜色缓冲区时使用的背景色。
```

#### `glClear`

当前用途：

```cpp
glClear(GL_COLOR_BUFFER_BIT);
```

作用：

```text
清空指定缓冲区。
```

当前只清空颜色缓冲区，也就是每帧开始时清掉上一帧画面。

### Buffer

#### `glGenBuffers`

项目位置：

```text
src/Engine/Renderer/VertexBuffer.cpp
src/Engine/Renderer/IndexBuffer.cpp
```

作用：

```text
向 OpenGL 申请 buffer object id。
```

#### `glBindBuffer`

当前用途：

```cpp
glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
```

作用：

```text
把某个 buffer object 绑定到当前 OpenGL 状态。
```

`GL_ARRAY_BUFFER` 通常是顶点数据，`GL_ELEMENT_ARRAY_BUFFER` 通常是索引数据。

#### `glBufferData`

作用：

```text
把 CPU 侧数组上传到当前绑定的 GPU buffer。
```

当前用途：

```cpp
glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
```

`GL_STATIC_DRAW` 表示这批数据上传后主要用于绘制，很少修改。

#### `glDeleteBuffers`

作用：

```text
删除 OpenGL buffer object。
```

通常放在 `VertexBuffer` / `IndexBuffer` 析构或 `Release()` 里。

### VertexArray

#### `glGenVertexArrays`

项目位置：

```text
src/Engine/Renderer/VertexArray.cpp
```

作用：

```text
向 OpenGL 申请 vertex array object id。
```

#### `glBindVertexArray`

作用：

```text
绑定一个 VAO，让后续顶点属性配置和 draw call 使用它。
```

VAO 记录的是：

```text
顶点属性启用状态
顶点属性如何从 VBO 读取
当前关联的 index buffer
```

#### `glEnableVertexAttribArray`

作用：

```text
启用某个 shader attribute location。
```

例如：

```glsl
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
```

C++ 侧就要启用 location 0 和 location 1。

#### `glVertexAttribPointer`

作用：

```text
告诉 OpenGL 如何从当前绑定的 VertexBuffer 里读取一个顶点属性。
```

例如：

```text
location 0:
    读取 3 个 float
    作为 position

location 1:
    读取 4 个 float
    作为 color
```

这一步是 `BufferLayout` 真正落到 OpenGL 的地方。

#### `glDeleteVertexArrays`

作用：

```text
删除 OpenGL vertex array object。
```

### Shader 编译和 program

#### `glCreateShader`

作用：

```text
创建一个 shader object。
```

常见类型：

```text
GL_VERTEX_SHADER
GL_FRAGMENT_SHADER
```

#### `glShaderSource`

作用：

```text
把 GLSL 源码字符串交给 shader object。
```

#### `glCompileShader`

作用：

```text
编译 shader object。
```

#### `glGetShaderiv`

当前用途：

```cpp
glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
```

作用：

```text
查询 shader object 的状态或信息。
```

#### `glGetShaderInfoLog`

作用：

```text
读取 shader 编译错误日志。
```

#### `glCreateProgram`

作用：

```text
创建 shader program。
```

program 是最终 draw call 使用的 GPU 程序。

#### `glAttachShader`

作用：

```text
把编译好的 vertex shader / fragment shader 附加到 program。
```

#### `glLinkProgram`

作用：

```text
把多个 shader stage 链接成一个完整 program。
```

#### `glGetProgramiv`

当前用途：

```cpp
glGetProgramiv(program, GL_LINK_STATUS, &success);
glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
```

作用：

```text
查询 program 链接状态或日志长度。
```

#### `glGetProgramInfoLog`

作用：

```text
读取 program 链接错误日志。
```

#### `glDetachShader`

作用：

```text
program 链接成功后，把 shader object 从 program 上分离。
```

#### `glDeleteShader`

作用：

```text
删除 shader object。
```

链接成功后，shader object 可以删除，program 仍然可用。

#### `glDeleteProgram`

作用：

```text
删除 shader program。
```

通常放在 `Shader` 析构或 `Release()` 里。

### Uniform

#### `glUniform4fv`

作用：

```text
向当前绑定的 shader program 上传 vec4 uniform。
```

当前可用于颜色、tint、材质参数等。

#### `glUniformMatrix4fv`

作用：

```text
向当前绑定的 shader program 上传 mat4 uniform。
```

摄像头功能里的 `u_ViewProjection` 就使用这个 API。

### 绘制

#### `glDrawElements`

项目位置：

```text
src/Engine/Renderer/RenderCommand.cpp
```

当前用途：

```cpp
glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
```

作用：

```text
根据当前绑定的 shader、VAO、index buffer 发起一次绘制。
```

参数含义：

```text
GL_TRIANGLES:
    每 3 个索引组成一个三角形。

count:
    要读取多少个索引。

GL_UNSIGNED_INT:
    index buffer 里的索引类型是 uint32_t。

nullptr:
    从当前 index buffer 起始位置读取。
```

## 4. 摄像头功能的完整调用链

摄像头矩阵参与渲染时，链路是：

```text
Window resize
    -> glViewport(...)
    -> Camera2D::SetViewportSize(...)

Layer / Renderer2D
    -> camera.GetViewProjection()
    -> Shader::Bind()
    -> Shader::SetMat4("u_ViewProjection", ...)
        -> glGetUniformLocation(...)
        -> glUniformMatrix4fv(...)

Draw
    -> VertexArray::Bind()
    -> RenderCommand::DrawIndexed(...)
        -> glDrawElements(...)
```

shader 侧：

```glsl
gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
```

这条链路的意义是：

```text
世界坐标
    -> Camera2D 计算矩阵
    -> glUniformMatrix4fv 上传到 GPU
    -> vertex shader 使用矩阵
    -> gl_Position 进入 OpenGL 后续管线
    -> glDrawElements 发起绘制
```

## 5. 记忆方式

可以把当前 OpenGL API 分成 5 类：

```text
窗口和画布：
    gladLoadGL
    glViewport

状态：
    glEnable
    glDisable
    glBlendFunc
    glUseProgram

资源：
    glGenBuffers
    glBufferData
    glGenVertexArrays
    glCreateShader
    glCreateProgram

数据连接：
    glVertexAttribPointer
    glGetUniformLocation
    glUniformMatrix4fv
    glUniform4fv

执行：
    glClear
    glDrawElements
```

当前阶段只需要理解调用链，不需要背完整 OpenGL API。
