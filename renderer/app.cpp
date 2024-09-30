#include <renderer/app.hpp>

#include <iostream>

namespace engine {

void App::Run()
{
    while(running_)
    {
        // main loop
        std::cout << "loop" << std::endl;
        renderer_.BeginFrame();
        auto command_buffer = renderer_.BeginRenderPass();
        renderer_.Render(command_buffer);
        renderer_.EndRenderPass();
        renderer_.EndFrame();
    }
}

} // namespace engine