# VSCode 可迁移开发指南

这个项目现在走“轻量 GitHub 仓库 + CMake 自动拉取固定依赖”的方案。

仓库里应该保留：

- 业务源码
- `CMakeLists.txt`
- `CMakePresets.json`
- `.vscode/tasks.json`
- `.vscode/settings.json`
- 文档

仓库里不应该保留：

- 完整第三方库源码
- 构建目录
- 本机绝对路径脚本
- Visual Studio / VSCode 的用户缓存
- `.exe`、`.dll`、`.lib`、`.pdb` 等编译产物

## 需要安装

- VSCode
- CMake 3.24+
- Ninja
- 一个 C++17 编译器
  - Windows：Visual Studio Build Tools 或 VS2022 自带 MSVC
  - Linux：GCC 或 Clang
  - macOS：AppleClang
- VSCode 扩展：C/C++、CMake Tools

Windows 上可以只使用 VS2022 的编译工具链，不需要打开 Visual Studio IDE。

## 命令行构建

```powershell
cmake --preset default
cmake --build --preset default
```

运行：

```powershell
.\build\default\TinyTacticsSandbox.exe
```

## VSCode 内构建

1. 用 VSCode 打开仓库根目录。
2. 安装 C/C++ 和 CMake Tools 扩展。
3. 按 `Ctrl+Shift+B` 执行默认构建任务。

`.vscode/tasks.json` 调用的是：

```powershell
cmake --preset default
cmake --build --preset default
```

所以它不会绑定你的用户名、磁盘路径或某个本机脚本。

## 依赖下载逻辑

`CMakeLists.txt` 使用 `FetchContent` 固定以下依赖版本：

- GLFW `3.4`
- GLAD `v2.0.8`
- glm `1.0.1`
- stb `f58f558c120e9b32c217290b80bad1a0729fbb2c`
- Dear ImGui `v1.91.9b`
- nlohmann/json `v3.11.3`

首次 `cmake --preset default` 会联网下载依赖到 `build/` 目录。后续构建会复用缓存，不会每次都重新下载。

## 迁移到新电脑

```powershell
git clone <your-repo-url>
cd openGL_demo
cmake --preset default
cmake --build --preset default
```

这就是完整迁移路径。你不需要把 `ThirdParty/` 推到 GitHub，也不需要手动复制依赖。

## 官方文档

- CMake Presets: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
- CMake FetchContent: https://cmake.org/cmake/help/latest/module/FetchContent.html
- VSCode C++ docs: https://code.visualstudio.com/docs/languages/cpp
