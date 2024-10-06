#pragma once

// std
#include <vector>
#include <memory>

// vulkan
#include <vulkan/vulkan.h>


// renderer includes
#include <renderer/window.hpp>

#include <renderer/renderer/device.hpp>
#include <renderer/renderer/swap_chain.hpp>
#include <renderer/renderer/pipeline.hpp>
#include <renderer/renderer/vertex_buffer.hpp>


namespace renderer
{

class Renderer
{
public:
    Renderer(engine::Window& window);
    ~Renderer();


    VkCommandBuffer BeginFrame();
    void BeginRenderPass(VkCommandBuffer command_buffer);
    void Render(VkCommandBuffer command_buffer);
    void EndRenderPass(VkCommandBuffer command_buffer);
    void EndFrame(VkCommandBuffer command_buffer);

    

private:
    void CreateCommandBuffers();
    void RecreateSwapChain();

private:
    engine::Window& window_;
    Device device_;
    SwapChain swap_chain_;
    Pipeline pipeline_; // triangle pipeline
    std::unique_ptr<VertexBuffer> vertex_buffer_ = nullptr;
    

    // image conuter == current frame index
    uint32_t current_image_index_ = 0;
    int current_frame_index_ = 0;

    // Commanfd buffers
    std::vector<VkCommandBuffer> command_buffers_;
};

} // namespace renderer
