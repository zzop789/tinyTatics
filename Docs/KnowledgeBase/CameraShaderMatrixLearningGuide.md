# Camera2D 和 Shader 矩阵学习笔记

## 1. 问题现象

在没有 `Camera2D` 之前，`FlatColor.vert` 可以直接写：

```glsl
gl_Position = vec4(a_Position, 1.0);
```

加入 `Camera2D` 后，shader 需要改成：

```glsl
uniform mat4 u_ViewProjection;
gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
```

容易产生的问题是：为什么 shader 要接收相机矩阵，直接使用顶点坐标不是也能画出来吗？

## 2. 根本原因

OpenGL 最终需要的是裁剪空间坐标，范围大致是：

```text
x: -1 到 1
y: -1 到 1
```

如果直接写：

```glsl
gl_Position = vec4(a_Position, 1.0);
```

意思是顶点数据本身就必须已经是裁剪空间坐标。例如：

```cpp
-0.5f, -0.5f
 0.5f,  0.5f
```

这些数值会直接对应屏幕里的位置。这种方式适合早期测试，但不适合真正的 2D 游戏坐标。

游戏里更希望用世界坐标表达对象，例如：

```text
单位在世界坐标 (10, 4)
地图格子大小为 1
相机看向地图的一部分
```

这时就需要相机矩阵把世界坐标转换成 OpenGL 需要的裁剪空间坐标。

## 3. 正确心智模型

没有相机矩阵时，顶点坐标直接等于屏幕裁剪坐标：

```text
顶点坐标 -> gl_Position -> 屏幕
```

有相机矩阵时，顶点坐标先被当成世界坐标，再由相机转换：

```text
世界坐标 -> ViewProjection 矩阵 -> gl_Position -> 屏幕
```

`u_ViewProjection` 通常由两部分组成：

```text
View 矩阵：
    表示相机位置。
    相机向右移动，画面里的世界看起来向左移动。

Projection 矩阵：
    表示相机能看到多大的范围，以及如何把这个范围映射到 -1 到 1。
```

所以：

```text
ViewProjection = Projection * View
```

shader 不负责“理解相机”，它只负责执行矩阵乘法。

## 4. 项目里的实践

当前项目里，`Camera2D` 应该负责保存：

```text
viewport width / height
position
zoom
view matrix
projection matrix
viewProjection matrix
```

`BaseLayer` 或未来的 `Renderer2D::BeginScene(camera)` 负责把矩阵传给 shader：

```cpp
m_Shader->Bind();
m_Shader->SetMat4("u_ViewProjection", m_Camera.GetViewProjection());
```

shader 里使用：

```glsl
uniform mat4 u_ViewProjection;
gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
```

这样应用层后面就可以用更自然的世界坐标：

```cpp
Renderer2D::DrawQuad({10.0f, 4.0f}, {1.0f, 1.0f}, color);
```

而不需要手动把所有对象坐标压缩到 `-1` 到 `1`。

## 5. 什么是 OpenGL 需要的裁剪空间坐标

裁剪空间坐标是 shader 写入 `gl_Position` 的坐标。OpenGL 后续会根据这个坐标判断一个顶点或三角形是否在可见范围内。

在最常见的理解方式里，经过透视除法之后，OpenGL 可见范围会变成：

```text
x: -1 到 1
y: -1 到 1
z: -1 到 1
```

这个范围也常被称为 NDC，也就是 Normalized Device Coordinates，归一化设备坐标。

可以先用 2D 的方式理解：

```text
(-1, -1) 约等于屏幕左下角
( 1, -1) 约等于屏幕右下角
( 1,  1) 约等于屏幕右上角
(-1,  1) 约等于屏幕左上角
( 0,  0) 是屏幕中心
```

如果一个点最终落在这个范围外，例如：

```text
x = 2.0
```

它就不在当前可见范围内，OpenGL 会把它裁剪掉，或者把穿过边界的三角形切开后只保留可见部分。

所以“裁剪空间”的重点是：

```text
OpenGL 不是直接理解游戏世界坐标。
OpenGL 最终需要 shader 输出一个能被裁剪和映射到屏幕的坐标。
```

没有相机矩阵时：

```glsl
gl_Position = vec4(a_Position, 1.0);
```

这表示 `a_Position` 已经被当成裁剪空间坐标使用。也就是你传入的坐标必须自己控制在 `-1` 到 `1` 附近。

有相机矩阵时：

```glsl
gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
```

这表示 `a_Position` 可以是世界坐标，相机矩阵负责把它转换成 OpenGL 需要的裁剪空间坐标。

## 6. 类似问题检查清单

- 如果只是验证 shader、buffer、VAO 是否能工作，可以暂时不用相机矩阵。
- 如果要做真正的 2D 场景、地图、单位、UI 世界坐标，就需要相机矩阵。
- 如果物体位置全部只能写在 `-1` 到 `1` 之间，说明还停留在裁剪空间测试阶段。
- 如果相机移动后画面没有变化，检查 `u_ViewProjection` 是否每帧传给 shader。
- 如果窗口比例变化后图形被拉伸，检查 `Camera2D::SetViewportSize(...)` 是否使用真实窗口尺寸。

## 7. 修改 `SetPosition` 后矩形消失的原因

当前 `BaseLayer::OnAttach()` 中的相机参数是：

```cpp
m_Camera.SetViewportSize(1280.0F, 720.0F);
m_Camera.SetPosition({ 0.0F, 0.0F });
m_Camera.SetZoom(1.0F);
```

`Camera2D::Recalculate()` 使用下面的可视范围：

```text
halfHeight = 1 / zoom
halfWidth  = halfHeight * (viewportWidth / viewportHeight)
```

代入当前数值：

```text
halfHeight = 1
halfWidth  = 1280 / 720 ≈ 1.78
```

因此相机位于 `(cameraX, cameraY)` 时，看到的世界范围约为：

```text
x: cameraX - 1.78 到 cameraX + 1.78
y: cameraY - 1.00 到 cameraY + 1.00
```

彩色矩形固定在世界原点附近：

```text
x: -0.5 到 0.5
y: -0.5 到 0.5
```

所以相机沿 x 方向移动超过约 `2.28`，或者沿 y 方向移动超过约 `1.5` 后，矩形会完全离开视野。这不是渲染失败，而是相机矩阵正确完成了裁剪。

### 相机移动方向

项目中的 View 矩阵使用：

```cpp
glm::translate(glm::mat4(1.0F), glm::vec3(-m_Position, 0.0F));
```

这里必须使用相机位置的负值：

```text
相机向右移动 -> 世界画面向左移动
相机向上移动 -> 世界画面向下移动
```

例如把相机从 `(0, 0)` 设置成 `(1, 0)`，预期结果是矩形向屏幕左侧移动，而不是向右移动。

### 设置位置与播放移动动画的区别

在 `OnAttach()` 中调用一次 `SetPosition()`，只会在程序启动时瞬间设置最终位置。它不会生成移动过程。

要观察连续移动，应该在每帧执行的 `OnUpdate(TimeStep timestep)` 中，根据帧间隔逐步修改位置。核心关系是：

```text
本帧移动距离 = 移动速度 * timestep.GetSeconds()
新位置 = 旧位置 + 本帧移动距离
```

在还没有接入键盘事件前，可以先用很小的位置验证静态结果：

```cpp
m_Camera.SetPosition({ 0.5F, 0.0F });
```

此时矩形应明显向左移动，但仍然处于可视范围内。确认静态移动正确后，再把位置更新放进 `OnUpdate()`，才会看到连续的镜头移动。
