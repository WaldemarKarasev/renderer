#include <renderer/app.hpp>

#include <iostream>

namespace engine {

void App::Run()
{
    while(running_)
    {
        // main loop
        std::cout << "loop" << std::endl;
        renderer.DrawFrame();
    }
}

} // namespace engine