# 本机工具链配置说明

项目仓库里的 `.vscode/settings.json` 和 `.vscode/tasks.json` 应尽量保持可迁移，不要写死某台电脑上的工具路径，例如：

```text
F:/cmake/bin/cmake.exe
F:/vsUsed/IDE/VC/Auxiliary/Build/vcvars64.bat
C:/Program Files (x86)/Windows Kits/...
```

这些路径属于“本机环境”，应该放在 VS Code 用户设置、系统 PATH，或者开发者命令行环境里。

## 1. CMake 和 Ninja

确保普通终端能执行：

```powershell
cmake --version
ninja --version
```

如果不能，就把对应目录加入用户 PATH。

你当前机器上类似是：

```text
F:\cmake\bin
F:\vsUsed\IDE\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja
```

## 2. MSVC 不建议手动加 cl.exe 到 PATH

MSVC 不只是一个 `cl.exe`。它还依赖这些环境变量：

```text
INCLUDE
LIB
LIBPATH
PATH
WindowsSdkDir
VCToolsInstallDir
```

所以不要只把：

```text
F:\vsUsed\IDE\VC\Tools\MSVC\...\bin\Hostx64\x64
```

加进 PATH。这样经常会出现能找到 `cl.exe`，但找不到标准库、Windows SDK 或链接库的问题。

## 3. 推荐方式 A：在 VS Code 里选择 CMake Kit

这是最推荐的方式，路径不会写进项目仓库。

操作：

```text
Ctrl+Shift+P
    -> CMake: Select a Kit
    -> 选择 Visual Studio / Build Tools / amd64 / x64 相关选项
```

你要找的名字通常类似：

```text
Visual Studio Build Tools 2022 Release - amd64
Visual Studio Community 2022 Release - amd64
Visual Studio Professional 2022 Release - amd64
```

选完以后再执行：

```text
Ctrl+Shift+P
    -> CMake: Configure
```

然后再构建：

```text
Ctrl+Shift+B
```

或者：

```text
Ctrl+Shift+P
    -> CMake: Build
```

如果 VS Code 没自动发现 Kit，可以执行：

```text
Ctrl+Shift+P
    -> CMake: Scan for Kits
```

## 4. 推荐方式 B：从 VS 开发者环境启动 VS Code

如果 CMake Tools 的 Kit 不好用，就用开发者环境启动 VS Code。

先打开：

```text
x64 Native Tools Command Prompt for VS 2022
```

或者手动执行：

```cmd
call F:\vsUsed\IDE\VC\Auxiliary\Build\vcvars64.bat
```

然后从这个终端启动 VS Code：

```cmd
cd /d F:\tiny
code .
```

这样 VS Code 继承到 MSVC 环境变量，项目里的通用 task：

```json
{
  "command": "cmake",
  "args": ["--preset", "default"]
}
```

就能找到 MSVC 编译环境。

## 5. 备用方式：使用 Visual Studio 生成器 preset

项目里还有一个 `vs2022` preset：

```powershell
cmake --preset vs2022
cmake --build --preset vs2022-debug
```

这个方式会生成 Visual Studio 工程，输出路径通常是：

```text
build/vs2022/Debug/TinyTacticsSandbox.exe
```

它比 Ninja preset 更依赖 Visual Studio 安装被 CMake 正确发现，但不要求你手动指定 Ninja。

## 6. 项目设置保持通用

仓库内 `.vscode/settings.json` 应保持类似这样：

```json
{
  "cmake.useCMakePresets": "always",
  "cmake.configureOnOpen": false,
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
  "C_Cpp.default.cppStandard": "c++17",
  "C_Cpp.default.compileCommands": "${workspaceFolder}/build/default/compile_commands.json"
}
```

本机路径应该放在：

```text
VS Code 用户设置
系统 PATH
CMake Tools Kit
Developer Command Prompt / PowerShell
```

不要提交到项目仓库里。
