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
    SwapChain(Device& device, VkExtent2D window_extent);
    ~SwapChain();

    VkResult AcquireImage(uint32_t* image_index);
    VkResult SubmitCommandBuffer(VkCommandBuffer command_buffer, uint32_t* image_index);

    // Getters
    VkRenderPass GetRenderPass() { return render_pass_; }
    VkFramebuffer GetFrameBuffer(uint32_t image_index) { return swap_chain_framebuffers_[image_index]; }
    VkExtent2D GetExtent() { return swap_chain_extent_; }

    void RecreateSwapChain(VkExtent2D new_window_extent);

private:
    void CreateSwapChain();
    void CreateRenderPass();
    void CreateDepthResources();
    void CreateImageViews(); 
    void CreateFramebuffers();
    void CreateSyncObjects();
    


private:
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkFormat FindDepthFormat();

private:
    Device& device_;
    VkExtent2D window_extent_;

    VkSwapchainKHR swap_chain_;

    // images
    std::vector<VkImage> depth_images_;
    std::vector<VkDeviceMemory> depth_image_memorys_;
    std::vector<VkImageView> depth_image_views_;

    std::vector<VkImage> swap_chain_images_;
    std::vector<VkImageView> swap_chain_image_views_;

    // SwapChain format end extent
    VkFormat swap_chain_image_format_;
    VkFormat swap_chain_depth_format_;
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