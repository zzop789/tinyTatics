#pragma once

// TinyTactics engine public entry header.
// Flow: Application/Sandbox code -> this header -> selected engine-facing APIs.
// Jump: add future public systems here first, then keep implementation headers private.

#include "Engine/Core/Application.h"
#include "Engine/Core/Layer.h"
#include "Engine/Core/TimeStep.h"
#include "Engine/Core/Window.h"
#include "Engine/Renderer/RenderCommand.h"
