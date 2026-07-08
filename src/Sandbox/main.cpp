#include "Engine/Core/core.h"
#include "Sandbox/Layers/baseLayer.h"

#include <cstdlib>
#include <iostream>
#include <memory>

int main()
{
    try
    {
        // Flow: Sandbox entry -> Application -> Window/RenderCommand.
        TinyTactics::Application application({
            "TinyTactics Sandbox",
            1280,
            720,
            true,
        });

        // Flow: Sandbox layer creation -> Application -> LayerStack ownership.
        application.PushLayer(std::make_unique<TinyTacticsSandbox::BaseLayer>());

        // Calls: Application::Run owns the frame loop.
        application.Run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
