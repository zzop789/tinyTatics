# 项目迁移指南

这份文档说明把项目上传到 GitHub 后，在另一台电脑上如何重新拉取、下载依赖、构建和运行。

## 仓库策略

本项目采用轻量仓库方案：

- GitHub 只保存源码、CMake 配置、VSCode 配置和文档。
- 不提交完整第三方库源码。
- 不提交 `build/`、`.exe`、`.dll`、`.lib`、`.pdb` 等构建产物。
- 第三方库由 CMake `FetchContent` 在首次配置时按固定版本自动下载。

这样仓库更小，迁移时也不会依赖某台电脑上的本地路径。

## 新电脑需要安装

### 必需

- Git
- CMake 3.24 或更高版本
- Ninja
- C++17 编译器

### Windows 推荐

- VSCode
- Visual Studio Build Tools 或 VS2022 自带的 MSVC 工具链
- VSCode 扩展：
  - C/C++
  - CMake Tools

不需要打开 Visual Studio IDE，只需要它提供编译器工具链。

## 迁移步骤

### 1. 克隆仓库

```powershell
git clone <your-repo-url>
cd openGL_demo
```

把 `<your-repo-url>` 换成你自己的 GitHub 仓库地址。

### 2. 配置项目

```powershell
cmake --preset default
```

这一步会自动下载固定版本的第三方库，包括：

- GLFW
- GLAD
- glm
- stb
- Dear ImGui
- nlohmann/json

依赖会下载到 `build/` 目录下。只要不删除 `build/`，后续不会每次都重新下载。

### 3. 构建项目

```powershell
cmake --build --preset default
```

### 4. 运行

Windows 下默认输出路径：

```powershell
.\build\default\TinyTacticsSandbox.exe
```

如果你使用的是 Visual Studio 生成器，输出路径可能是：

```powershell
.\build\vs2022\Debug\TinyTacticsSandbox.exe
```

## VSCode 使用方式

1. 用 VSCode 打开仓库根目录。
2. 安装 `C/C++` 和 `CMake Tools` 扩展。
3. 按 `Ctrl+Shift+B` 构建。

VSCode 任务会执行：

```powershell
cmake --preset default
cmake --build --preset default
```

## 如果首次下载依赖失败

常见原因是网络、代理或 GitHub 访问不稳定。

可以尝试：

1. 重新执行：

```powershell
cmake --preset default
```

2. 删除构建目录后重试：

```powershell
Remove-Item -Recurse -Force build
cmake --preset default
```

3. 检查 GitHub 是否能访问：

```powershell
git ls-remote https://github.com/glfw/glfw.git
```

如果这个命令失败，说明问题在网络或 Git 配置，不是项目配置。

## 不要上传的内容

上传 GitHub 前确认这些内容没有被提交：

- `build/`
- `ThirdParty/`
- `.vs/`
- `.exe`
- `.dll`
- `.lib`
- `.pdb`
- CMake 临时缓存文件

这些内容已经写入 `.gitignore`。

可以用下面命令检查：

```powershell
git status --short
```

正常情况下，应该只看到源码、配置和文档文件。

## 推荐提交内容

建议提交：

- `.gitignore`
- `CMakeLists.txt`
- `CMakePresets.json`
- `.vscode/tasks.json`
- `.vscode/settings.json`
- `src/`
- `Docs/`
- `tinyTactis.md`

## 官方文档

- CMake FetchContent: https://cmake.org/cmake/help/latest/module/FetchContent.html
- CMake Presets: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html
- GLFW documentation: https://www.glfw.org/docs/latest/
- GLAD GitHub: https://github.com/Dav1dde/glad
- glm manual: https://github.com/g-truc/glm/blob/master/manual.md
- Dear ImGui: https://github.com/ocornut/imgui
- nlohmann/json: https://json.nlohmann.me/
