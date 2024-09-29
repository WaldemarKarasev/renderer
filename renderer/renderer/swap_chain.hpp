#pragma once

// std
#include <vector>

// vulkan 
#include <vulkan/vulkan.h>

// renderer includes
#include <renderer/renderer/device.hpp>

namespace renderer {

class SwapChain
{
public:
    SwapChain(engine::Window& window, Device& device);
    ~SwapChain();
private:
    void CreateSwapChain();
    void CreateSwapChainImages(); 

private:
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

private:
    engine::Window& window_;
    Device& device_;

    VkSwapchainKHR swap_chain_;
    std::vector<VkImage> swap_chain_images_;
    VkFormat swap_chain_image_format_;
    VkExtent2D swap_chain_extent_;

    // ImageViews
    std::vector<VkImageView> swap_chain_image_views_;

};


} // namespace renderer