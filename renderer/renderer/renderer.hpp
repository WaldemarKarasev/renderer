#pragma once

// std

// vulkan

// renderer includes
#include <renderer/window.hpp>

#include <renderer/renderer/device.hpp>
#include <renderer/renderer/swap_chain.hpp>
#include <renderer/renderer/pipeline.hpp>


#define MAX_FRAMES_IN_FLIGHT 2

namespace renderer
{

class Renderer
{
public:
    Renderer(engine::Window& window);
    ~Renderer();
    void DrawFrame();

private:
    void CreateFramebuffers();
    void CreateCommapdPool();
    void CreateCommandBuffers();
    void CreateSyncObjects();

    void RecordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index);
    void RecreateSwapChain();
private:
    engine::Window& window_;
    Device device_;
    SwapChain swap_chain_;
    Pipeline pipeline_;

    // Framebuffers
    std::vector<VkFramebuffer> swap_chain_framebuffers_;

    // Commanfd buffers
    VkCommandPool command_pool_;
    std::vector<VkCommandBuffer> command_buffers_;

    // Sync objects
    std::vector<VkSemaphore> image_available_semaphores_;
    std::vector<VkSemaphore> render_finished_semaphores_;
    std::vector<VkFence> in_flight_fences_;
    uint32_t current_frame_ = 0;

    bool framebuffer_resized_ = false;

};

} // namespace renderer
