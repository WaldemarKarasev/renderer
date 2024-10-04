#include <renderer/app.hpp>

#include <iostream>

namespace engine {

void App::Run()
{
    while(!window_.ShouldClose())
    {
        // main loop
        glfwPollEvents();
        
        std::cout << "loop" << std::endl;
        if (auto command_buffer = renderer_.BeginFrame())
        {
            renderer_.BeginRenderPass();
            renderer_.Render(command_buffer);
            renderer_.EndRenderPass(command_buffer);
            renderer_.EndFrame(command_buffer);
        }
        else
        {
            std::cout << "Error receiving command_buffer from BeginFrame function" << std::endl;
        }
    }
}

} // namespace engine