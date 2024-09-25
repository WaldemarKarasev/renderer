#include <renderer/app.hpp>

#include <iostream>

namespace engine {

void App::Run()
{
    while(running_)
    {
        std::cout << "loop" << std::endl;
        // main loop
    }
}

} // namespace engine