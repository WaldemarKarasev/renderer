#pragma once 

#include <renderer/window.hpp>
#include <renderer/renderer/device.hpp>
#include <renderer/renderer/renderer.hpp>
namespace engine {

class App
{
public:
    void Run();

private:
    bool running_{true};

    engine::Window window_{"Triangle", 1920, 1080};
    renderer::Renderer renderer_{window_};
};

} // namespace renderer