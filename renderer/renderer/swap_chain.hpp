#pragma once

// std
#include <vector>

// vulkan 
#include <vulkan/vulkan.h>

// renderer includes
#include <renderer/renderer/device.hpp>




namespace renderer {


struct SwapChainInfo
{
    // render pass
    VkRenderPass render_pass_;
};

class SwapChain
{
public:
    static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;


public:
    SwapChain(engine::Window& window, Device& device);
    ~SwapChain();


    SwapChainInfo GetSwapChainInfo() { return SwapChainInfo{render_pass_}; }

    VkResult AquireImage(uint32_t* image_index);
    VkResult SubmitCommandBuffer(VkCommandBuffer command_buffer, uint32_t* image_index);

    // Getters
    VkRenderPass GetRenderPass() { return render_pass_; }
    VkFramebuffer GetFrameBuffer(uint32_t image_index) { return swap_chain_framebuffers_[image_index]; }
    VkExtent2D GetExtent() { return swap_chain_extent_; }

    void RecreateSwapChain();
private:
    void CreateSwapChain();
    void CleanupSwapChain();

    void CreateSwapChainImageViews(); 



    void CreateFramebuffers();
    void CreateSyncObjects();

    void RecordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index);

    void CreateRenderPass();

private:
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


private:
    engine::Window& window_;
    Device& device_;

    VkSwapchainKHR swap_chain_;
    std::vector<VkImage> swap_chain_images_;
    // ImageViews
    std::vector<VkImageView> swap_chain_image_views_;

    // SwapChain format end extent
    VkFormat swap_chain_image_format_;
    VkExtent2D swap_chain_extent_;

    // Framebuffers
    std::vector<VkFramebuffer> swap_chain_framebuffers_;

    // Sync objects
    std::vector<VkSemaphore> image_available_semaphores_;
    std::vector<VkSemaphore> render_finished_semaphores_;
    std::vector<VkFence> in_flight_fences_;
    uint32_t current_frame_ = 0;

    // render pass
    VkRenderPass render_pass_;
};


} // namespace renderer