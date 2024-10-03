#pragma once

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>


// renderer includes
#include <renderer/window.hpp>

#include <renderer/renderer/device.hpp>
#include <renderer/renderer/swap_chain.hpp>
#include <renderer/renderer/pipeline.hpp>



namespace renderer
{

class Renderer
{
public:
    Renderer(engine::Window& window);
    ~Renderer();


    VkCommandBuffer BeginFrame();
    void BeginRenderPass();
    void Render(VkCommandBuffer command_buffer);
    void EndRenderPass();
    void EndFrame(VkCommandBuffer command_buffer);

    

private:
    void CreateCommandBuffers();
    void RecreateSwapChain();

private:
    engine::Window& window_;
    Device device_;
    SwapChain swap_chain_;
    Pipeline pipeline_; // triangle pipeline
    

    // image conuter == current frame index
    uint32_t current_image_index_ = 0;

    // Commanfd buffers
    std::vector<VkCommandBuffer> command_buffers_;
};

} // namespace renderer
