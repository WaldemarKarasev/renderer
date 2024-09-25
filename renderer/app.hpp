#pragma once 

#include <renderer/window.hpp>
#include <renderer/renderer/device.hpp>

namespace engine {

class App
{
public:
    void Run();

private:
    bool running_{false};

    engine::Window window{"Triangle", 1920, 1080};
    renderer::Device device_{window};

};

} // namespace renderer