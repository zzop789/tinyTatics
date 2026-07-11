# VSCode 构建指南

本项目不再依赖本机专属脚本，也不写死 VS2022 安装目录。VSCode 只负责调用 CMake Presets。

## 准备环境

- VSCode
- CMake 3.24+
- Ninja
- C++17 编译器
  - Windows：MSVC / Visual Studio Build Tools / VS2022 自带编译工具
  - Linux：GCC 或 Clang
  - macOS：AppleClang
- VSCode 扩展：C/C++、CMake Tools

## VSCode 构建

1. 用 VSCode 打开仓库根目录。
2. 按 `Ctrl+Shift+B`。
3. 默认任务会执行：

```powershell
cmake --preset default
cmake --build --preset default
```

构建产物位于：

```text
build/default/
```

Windows 下可执行文件通常是：

```text
build/default/TinyTacticsSandbox.exe
```

## 命令行构建

```powershell
cmake --preset default
cmake --build --preset default
```

运行：

```powershell
.\build\default\TinyTacticsSandbox.exe
```

## 依赖说明

首次配置时，CMake 会通过 `FetchContent` 下载固定版本的 GLFW、GLAD、glm、stb、Dear ImGui 和 nlohmann/json。

这些依赖会缓存在 `build/` 目录中。只要不删除 `build/`，后续构建不会重复下载。
