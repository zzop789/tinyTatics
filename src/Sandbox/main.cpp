#include "Engine/Core/core.h"
#include "Sandbox/Layers/baseLayer.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

namespace
{
    float ReadAutoCloseArgument(int argc, char** argv)
    {
        constexpr const char* prefix = "--auto-close=";

        for (int index = 1; index < argc; ++index)
        {
            if (std::strncmp(argv[index], prefix, std::strlen(prefix)) == 0)
            {
                return static_cast<float>(std::atof(argv[index] + std::strlen(prefix)));
            }
        }

        return 0.0F;
    }
}

int main(int argc, char** argv)
{
    try
    {
        // Flow: Sandbox entry -> Application -> Window/RenderCommand.
        TinyTactics::Application application({
            "TinyTactics Sandbox",
            1280,
            720,
            true,
            ReadAutoCloseArgument(argc, argv),
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
