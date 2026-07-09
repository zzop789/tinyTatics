# Shader Learning Notes

## Current Rule

For now, treat `Shader` as a black box while the renderer pipeline is being built.

If a question is about shader internals before the dedicated shader-learning pass, record it here and continue with the minimal operational explanation needed to proceed.

## Deferred Questions

- 2026-07-10: What exactly do `Shader::Bind()` and `Shader::Unbind()` do internally?
  - Short working answer: they select or clear the current OpenGL shader program with `glUseProgram(...)`.
  - Deferred detail: OpenGL program state, shader program lifecycle, compile/link stages, and why binding is needed before draw calls.

