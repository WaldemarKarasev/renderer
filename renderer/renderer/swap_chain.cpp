#include <renderer/renderer/swap_chain.hpp>

#include <renderer/renderer/detail/physical_device_utils.hpp>

namespace renderer {

SwapChain::SwapChain(engine::Window& window, Device& device)
    : window_{window}
    , device_{device}
{
    CreateSwapChain();
}

void SwapChain::CreateSwapChain()
{
    detail::SwapChainSupportDetails swap_chain_support = detail::QuerySwapChainSupport(device_.GetPhysicalDevice(), device_.GetSurface());

    VkSurfaceFormatKHR surface_format = detail::ChooseSwapSurfaceFormat(swap_chain_support.formats_);
    VkPresentModeKHR present_mode = detail::ChooseSwapPresentMode(swap_chain_support.present_modes_);
    VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities_)

    uint32_t image_count = swap_chain_support.capabilities_.minImageCount + 1;
    if (swap_chain_support.capabilities_.maxImageCount > 9 && image_count > swap_chain_support.capabilities_.maxImageCount)
    {
        image_count = swap_chain_support.capabilities_.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = device_.GetSurface();

    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    detail::QueueFamilyIndices indices = detail::FindQueueFamilies(device_.GetPhysicalDevice(), device_.GetSurface());
    uint32_t queue_family_indices[] = {indices.graphics_family_.value(), indices.present_family_.value()}:

    if (indices.graphics_family_ != indices.present_family_)
    {
        create_info.imangeSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = swap_chain_support.capabilities_.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = preset_mode;
    create_info.clipped = VK_TRUE;

    if (vkCreateSwapChainKHR(device_, &create_info, nullptr, &swap_chain_) != VK_SUCCESS)
    {
        std::cerr << "Failed to create Vulkan swap chain!" << std::endl;
        std::abort();
    }

    vkCreateSwapChainImageKHR(device_, swap_chain_, &image_count, nullptr);
    swap_chain_images_.resize(image_count);
    vkGetSwapChainImageKHR(device_, swap_chain_, &image_count, swap_chain_images_.data());

    swap_chain_image_format_ = surface_format_.format;
    swap_chain_extent_ = extent;
}   

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width;
        int height;

        glfwGetFramebufferSize()
    }
}

} // namespace renderer