# C++ 资源所有权学习笔记

## 1. 问题现象

在 `Texture2D` 这类渲染资源封装里，会看到类似代码：

```cpp
explicit Texture2D(const std::string& path);
~Texture2D();

Texture2D(const Texture2D&) = delete;
Texture2D& operator=(const Texture2D&) = delete;
```

容易产生两个问题：

- 为什么构造函数要加 `explicit`？
- 为什么要删除拷贝构造和拷贝赋值？

## 2. 根本原因

`Texture2D` 不是普通数据对象，它拥有一个 OpenGL texture id。

这个 id 通常来自：

```cpp
glGenTextures(...)
```

并且最终需要：

```cpp
glDeleteTextures(...)
```

这意味着 `Texture2D` 是一个资源所有者。它的对象生命周期和 GPU 资源生命周期绑定在一起。

如果允许默认拷贝，两个 `Texture2D` 对象可能会持有同一个 OpenGL texture id：

```text
Texture2D A -> id 7
Texture2D B -> id 7
```

当两个对象析构时，就可能发生两次释放：

```text
A 析构 -> glDeleteTextures(7)
B 析构 -> glDeleteTextures(7)
```

这会造成资源生命周期错误。

## 3. `explicit` 的正确理解

`explicit` 用在单参数构造函数上，用来禁止隐式转换。

没有 `explicit` 时，下面这种代码可能成立：

```cpp
void UseTexture(Texture2D texture);

UseTexture("Assets/Textures/test.png");
```

编译器可能会自动把字符串路径转换成一个临时 `Texture2D`。

这对资源对象不理想，因为创建 texture 是一个有成本、有副作用的操作：

```text
读取图片文件
分配 CPU 内存
创建 OpenGL texture
上传图片到 GPU
```

加上 `explicit` 后，调用者必须明确写：

```cpp
Texture2D texture("Assets/Textures/test.png");
```

或者：

```cpp
auto texture = std::make_unique<Texture2D>("Assets/Textures/test.png");
```

这让“加载一张贴图”这个动作在代码里更明确。

## 4. 删除拷贝构造和拷贝赋值

这两行删除的是拷贝行为：

```cpp
Texture2D(const Texture2D&) = delete;
Texture2D& operator=(const Texture2D&) = delete;
```

它们分别表示：

```text
禁止用一个 Texture2D 创建另一个 Texture2D。
禁止把一个 Texture2D 赋值给另一个 Texture2D。
```

例如这些代码会被禁止：

```cpp
Texture2D a("Assets/Textures/test.png");
Texture2D b = a;      // 禁止

Texture2D c("Assets/Textures/other.png");
c = a;                // 禁止
```

原因是 `Texture2D` 拥有 OpenGL texture id，默认拷贝只会复制 id 数字，不会真正复制 GPU 资源。

## 5. 拷贝和移动不是一回事

`const Texture2D&` 对应的是拷贝语义：

```cpp
Texture2D(const Texture2D&) = delete;
```

`Texture2D&&` 对应的是移动语义：

```cpp
Texture2D(Texture2D&& other) noexcept;
Texture2D& operator=(Texture2D&& other) noexcept;
```

移动不是复制资源，而是转移所有权：

```text
移动前：
    A -> id 7
    B -> id 0

移动后：
    A -> id 0
    B -> id 7
```

这样最终仍然只有一个对象负责释放 `id 7`。

所以资源对象常见规则是：

```text
禁止拷贝
允许移动
析构释放资源
```

当前 `Texture2D` 如果只写了删除拷贝，也可以先工作；后面如果需要把 `Texture2D` 放进容器或从函数返回，再补移动构造和移动赋值。

## 6. 项目里的实践

对 `Texture2D`、`Shader`、`VertexBuffer`、`IndexBuffer`、`VertexArray` 这类 OpenGL 资源封装，推荐默认遵守：

```cpp
ClassName(const ClassName&) = delete;
ClassName& operator=(const ClassName&) = delete;

ClassName(ClassName&& other) noexcept;
ClassName& operator=(ClassName&& other) noexcept;
```

构造函数如果只接一个路径或一个资源描述参数，优先加 `explicit`：

```cpp
explicit Texture2D(const std::string& path);
explicit Shader(const std::string& path);
```

如果构造函数天然需要多个参数，例如：

```cpp
Shader(const std::string& vertexPath, const std::string& fragmentPath);
```

隐式转换风险小很多，但仍然可以按项目风格决定是否加 `explicit`。

## 7. 检查清单

- 这个类是否拥有 OpenGL id、文件句柄、内存块、socket、线程等资源？
- 如果两个对象拷贝后持有同一个资源 id，会不会双重释放？
- 如果会双重释放，就删除拷贝构造和拷贝赋值。
- 如果需要转移所有权，就实现移动构造和移动赋值。
- 单参数构造函数如果会触发资源加载，优先加 `explicit`。

