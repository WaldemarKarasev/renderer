#pragma once 

#include "renderer/device.hpp"

namespace engine {

class App
{
public:
    void Run();

private:
    bool running_{false};

    renderer::Device device_;

};

} // namespace renderer