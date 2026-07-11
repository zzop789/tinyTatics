# VS Code 调试器 DLL 加载信息理解笔记

## 1. 现象

在使用 VS Code 的 `cppvsdbg` 调试 `TinyTacticsSandbox.exe` 时，调试控制台可能反复出现：

```text
Loaded 'C:\Windows\System32\psapi.dll'.
Loaded 'C:\Windows\System32\dbghelp.dll'.
Unloaded 'C:\Windows\System32\psapi.dll'.
Unloaded 'C:\Windows\System32\dbghelp.dll'.
```

这些行是模块加载跟踪信息，不等同于编译错误、运行时异常或内存泄漏。

## 2. 这些 DLL 是什么

- `psapi.dll`：Windows 的进程状态 API，能够查询进程和已加载模块的信息。
- `dbghelp.dll`：Windows 的调试辅助库，用于符号、调用栈和崩溃诊断等功能。

调试器、显卡驱动或其他运行时组件可以按需加载这些系统 DLL，完成一次查询后再卸载，所以可能看到多次 `Loaded` 和 `Unloaded`。

## 3. 正确判断方式

只出现 `Loaded` / `Unloaded` 时，通常不需要处理。真正需要关注的是附近是否同时出现以下信息：

- `Exception thrown` 或“引发异常”；
- `Access violation`；
- `HEAP CORRUPTION`；
- 非零进程退出码；
- 程序窗口无响应或异常关闭；
- 明确指出某个项目源码文件和行号的调用栈。

判断原则是：模块被卸载不代表 DLL 出错，必须结合异常文本、退出码和程序行为判断。

## 4. 本项目检查结果

项目源码与 `.vscode/launch.json` 中没有主动调用 `LoadLibrary`、`FreeLibrary`、`DbgHelp` 或 `PSAPI`。因此当前这些输出不是 TinyTactics 引擎代码主动反复装卸 DLL。

检查期间，旧构建产物曾返回 `0xC000041D`。使用完整 MSVC 环境执行干净重建后，自动关闭测试正常返回 `EXIT_CODE=0`：

```powershell
cmd.exe /d /c scripts\cmake-msvc-env.cmd --build --preset default --clean-first
```

这说明 DLL 加载日志本身不是故障；旧构建状态与正常调试日志应分开判断。

## 5. 同类问题检查清单

1. 先看程序窗口是否能正常运行和关闭。
2. 搜索调试输出中的 `Exception`、`Access violation`、`HEAP CORRUPTION` 和退出码。
3. 只有 `Loaded` / `Unloaded` 时，先视为调试信息。
4. 怀疑构建产物与源码不同步时，使用 `--clean-first` 完整重建。
5. 重建后再次运行，确认进程退出码是否为 `0`。

## 6. 如何避免旧构建影响调试

### 6.1 不建议每次调试都清理

当前 `.vscode/launch.json` 使用 `CMake: build` 作为 `preLaunchTask`，而该任务会先配置再构建。因此按下 `F5` 时，CMake/Ninja 会根据依赖关系和文件时间戳重新编译发生变化的源码，然后才启动程序。

正常增量构建不会把旧 `.obj` 随意混入新程序。每次都使用 `--clean-first` 虽然简单，但会重新编译引擎和第三方库，明显增加调试等待时间。

推荐保留两种入口：

```text
日常调试     -> 增量构建 -> 启动程序
怀疑缓存异常 -> 干净重建 -> 启动程序
```

### 6.2 三种清理等级

第一等级：普通增量构建，适用于日常修改 `.h/.cpp`：

```powershell
cmd.exe /d /c scripts\cmake-msvc-env.cmd --build --preset default
```

第二等级：清理编译产物后重建，适用于源码已修改但运行行为仍像旧版本：

```powershell
cmd.exe /d /c scripts\cmake-msvc-env.cmd --build --preset default --clean-first
```

第三等级：重新生成 CMake 缓存，适用于更换编译器、生成器、SDK 或大幅修改依赖配置：

```powershell
cmd.exe /d /c scripts\cmake-msvc-env.cmd --fresh --preset default
cmd.exe /d /c scripts\cmake-msvc-env.cmd --build --preset default
```

`--fresh` 比 `--clean-first` 更彻底：前者重新生成 `CMakeCache.txt`，后者只清理编译和链接产物。

### 6.3 容易被误认为“旧构建”的情况

- 上一次程序或 CRT 错误对话框仍在运行，导致链接器无法替换 `.exe` 或 `.pdb`。
- 修改文件后没有保存，构建系统读取到的仍是磁盘上的旧内容。
- MSVC、Clang 或不同生成器共用同一个构建目录。
- 文件从其他设备复制过来后带有异常时间戳，Ninja 误判文件没有变化。
- 调试配置启动的 `program` 路径与实际构建输出目录不一致。
- CMake 构建失败，但随后手动启动了上一次成功生成的 `.exe`。

### 6.4 TinyTactics 的建议

日常继续使用现有 `Debug TinyTacticsSandbox` 配置。遇到明显的源码与运行结果不一致时，先关闭旧窗口和 CRT 错误对话框，再执行一次 `--clean-first`。

如果以后同时使用 MSVC 和 Clang，应为它们设置独立目录，例如：

```text
build/msvc-debug/
build/clang-debug/
```

不要让不同编译器共享当前的 `build/default/`。这比每次调试都清空所有构建产物更稳定，也保留了增量编译速度。
