# TinyTactics 文档索引

文档现在分成两类：

```text
Docs/Guides/
Docs/KnowledgeBase/
```

## Guides

`Guides` 放操作型文档：环境配置、迁移、依赖安装、渲染路线、工具使用。  
特点是回答“我现在要怎么做”。

- [LocalToolchainGuide.md](Guides/LocalToolchainGuide.md)：本机 CMake / Ninja / MSVC 工具链说明。
- [MigrationGuide.md](Guides/MigrationGuide.md)：项目迁移和继续开发说明。
- [OpenGLDependencyGuide.md](Guides/OpenGLDependencyGuide.md)：OpenGL 相关依赖说明。
- [PortableVSCodeGuide.md](Guides/PortableVSCodeGuide.md)：VS Code 可迁移配置说明。
- [RenderingStepGuide.md](Guides/RenderingStepGuide.md)：渲染系统推进路线。
- [ShaderGuide.md](Guides/ShaderGuide.md)：`Shader.h/.cpp` 使用和理解指南。
- [VSCodeGuide.md](Guides/VSCodeGuide.md)：VS Code 使用说明。

## KnowledgeBase

`KnowledgeBase` 放学习笔记和问题复盘：概念解释、错误成因、调试经验、知识归纳。  
特点是回答“为什么是这样”和“以后遇到同类问题怎么理解”。

- [CameraShaderMatrixLearningGuide.md](KnowledgeBase/CameraShaderMatrixLearningGuide.md)：Camera 与 Shader 矩阵关系学习笔记。
- [DebuggerModuleLoadingMessagesGuide.md](KnowledgeBase/DebuggerModuleLoadingMessagesGuide.md)：调试器 DLL 加载与卸载信息判断方法。
- [OpenGLApiLearningGuide.md](KnowledgeBase/OpenGLApiLearningGuide.md)：OpenGL API 学习笔记。
- [ShaderLearningNotes.md](KnowledgeBase/ShaderLearningNotes.md)：Shader 学习笔记。
- [ShutdownHeapCorruptionGuide.md](KnowledgeBase/ShutdownHeapCorruptionGuide.md)：关闭窗口 heap corruption 问题复盘。

## 归档规则

新增文档时按这个规则放：

```text
要照着执行的流程、步骤、配置 -> Docs/Guides/
要反复学习的概念、错误复盘 -> Docs/KnowledgeBase/
```
