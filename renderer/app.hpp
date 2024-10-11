#pragma once 

// std
#include <memory>

// vulkan
#include <vulkan/vulkan.h>

// renderer includes
#include <renderer/window.hpp>
#include <renderer/renderer/device.hpp>
#include <renderer/renderer/descriptors.hpp>
#include <renderer/renderer/renderer.hpp>
#include <renderer/renderer/pipeline.hpp>
#include <renderer/renderer/frame_utility.hpp>

namespace engine {

class App
{
public:
    App();

    void Run();

    void Render(const renderer::FrameInfo& frame_info);


private:
    void UpdateUBO(const renderer::FrameInfo& frame_info);

private:
    bool running_{true};

    engine::Window window_{"Triangle", 1920, 1080};
    renderer::Device device_{window_};
    renderer::Renderer renderer_{device_, window_};

    std::unique_ptr<renderer::DescriptorPool> global_descriptor_pool_ = nullptr;
    // for rendering once quad. For demo only
    std::unique_ptr<renderer::Pipeline> pipeline_ = nullptr; // triangle pipeline
    std::unique_ptr<renderer::VertexBuffer> vertex_buffer_ = nullptr;

    std::vector<std::unique_ptr<renderer::Buffer>> ubo_buffers;
    std::unique_ptr<renderer::DescriptorSetLayout> global_descriptor_set_layout_ = nullptr;
    std::vector<VkDescriptorSet> global_descriptor_sets_;

};

} // namespace renderer