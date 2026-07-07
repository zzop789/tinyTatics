# OpenGL 依赖方案

本项目采用轻量仓库方案：GitHub 只提交源码、CMake 配置、VSCode 配置和文档，不提交完整第三方库源码，也不提交编译产物。

首次配置时，CMake 会通过 `FetchContent` 下载固定版本的依赖。下载完成后，这些依赖会缓存在 `build/` 目录下，后续构建不会反复下载，除非你删除了构建目录。

## 当前依赖

| Library | Version / Tag | Purpose |
| --- | --- | --- |
| GLFW | `3.4` | 窗口、输入、OpenGL Context |
| GLAD | `v2.0.8` | OpenGL 3.3 Core 函数加载 |
| glm | `1.0.1` | 数学类型和矩阵 |
| stb | `f58f558c120e9b32c217290b80bad1a0729fbb2c` | 图片加载等单头文件工具 |
| Dear ImGui | `v1.91.9b` | 调试 / 编辑器 UI |
| nlohmann/json | `v3.11.3` | JSON 配置 |

## 构建

推荐使用 CMake Presets：

```powershell
cmake --preset default
cmake --build --preset default
.\build\default\TinyTacticsSandbox.exe
```

如果使用 Visual Studio 2022 生成器：

```powershell
cmake --preset vs2022
cmake --build --preset vs2022-debug
.\build\vs2022\Debug\TinyTacticsSandbox.exe
```

## 迁移方式

新电脑上只需要：

```powershell
git clone <your-repo-url>
cd openGL_demo
cmake --preset default
cmake --build --preset default
```

不需要手动下载 `ThirdParty/`。CMake 会按 `CMakeLists.txt` 中固定的 tag / commit 拉取同一批依赖。

## 不提交到 GitHub 的内容

- `ThirdParty/`
- `build/`
- `.vs/`
- CMake 生成缓存
- `.exe`、`.dll`、`.lib`、`.pdb` 等构建产物

这些内容已经由 `.gitignore` 排除。

## 官方文档

- CMake FetchContent: https://cmake.org/cmake/help/latest/module/FetchContent.html
- CMake Presets: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
- OpenGL Registry: https://registry.khronos.org/OpenGL/
- OpenGL Wiki: https://www.khronos.org/opengl/wiki/
- GLFW documentation: https://www.glfw.org/docs/latest/
- GLAD generator: https://gen.glad.sh/
- GLAD GitHub: https://github.com/Dav1dde/glad
- glm manual/API: https://github.com/g-truc/glm/blob/master/manual.md
- stb GitHub: https://github.com/nothings/stb
- Dear ImGui GitHub: https://github.com/ocornut/imgui
- nlohmann/json documentation: https://json.nlohmann.me/
