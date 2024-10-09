#pragma once 

// std
#include <memory>

// vulkan
#include <vulkan/vulkan.h"

// renderer includes
#include <renderer/window.hpp>
#include <renderer/renderer/device.hpp>
#include <renderer/renderer/renderer.hpp>

#include <renderer/renderer/pipeline.hpp>

namespace engine {

class App
{
public:
    App();

    void Run();

    void Render(VkCommandBuffer command_buffer);

private:
    bool running_{true};

    engine::Window window_{"Triangle", 1920, 1080};
    renderer::Device device_{window_};
    renderer::Renderer renderer_{device_, window_};

    std::unique_ptr<DescriptorPool> glob_descriptor_pool_;

    // for rendering once quad. For demo only
    std::unique_ptr<Pipeline> pipeline_ = nullptr; // triangle pipeline
    std::unique_ptr<VertexBuffer> vertex_buffer_ = nullptr;
};

} // namespace renderer