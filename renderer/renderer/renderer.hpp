#pragma once

// std

// vulkan
#incldue <vulkan/vulkan.h>


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
    void EndFrame();

private:
    void CreateCommandBuffers();


private:
    engine::Window& window_;
    Device device_;
    SwapChain swap_chain_;
    Pipeline pipeline_;
    
    bool framebuffer_resized_ = false;


    // Commanfd buffers
    std::vector<VkCommandBuffer> command_buffers_;
};

} // namespace renderer
