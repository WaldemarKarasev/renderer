#pragma once

// std
#include <memory>
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

// renderer includes
#include <renderer/window.hpp>

#include <renderer/renderer/device.hpp>
#include <renderer/renderer/descriptors.hpp>
#include <renderer/renderer/pipeline.hpp>
#include <renderer/renderer/swap_chain.hpp>
#include <renderer/renderer/vertex_buffer.hpp>

namespace renderer
{

class Renderer
{
public:
    Renderer(Device& device, engine::Window& window);
    ~Renderer();


    VkCommandBuffer BeginFrame();
    void BeginRenderPass(VkCommandBuffer command_buffer);
    void EndRenderPass(VkCommandBuffer command_buffer);
    void EndFrame(VkCommandBuffer command_buffer);

    VkRenderPass GetSwapchainRenderPass() { return swap_chain_.GetRenderPass(); }

private:
    void CreateCommandBuffers();
    void RecreateSwapChain();

private:
    engine::Window& window_;
    Device& device_;
    SwapChain swap_chain_;   

    // image conuter == current frame index
    uint32_t current_image_index_ = 0;
    int current_frame_index_ = 0;

    // Commanfd buffers
    std::vector<VkCommandBuffer> command_buffers_;
};

} // namespace renderer
