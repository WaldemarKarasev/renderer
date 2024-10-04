#pragma once

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

namespace renderer::detail {

inline VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
{
    VkSurfaceFormatKHR selected_format;
    for (const auto& available_format : available_formats)
    {
        if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            selected_format = available_format;
            break;
        }
    }

    return selected_format;
}

inline VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
{
    for (const auto& available_present_mode : available_present_modes)
    {
        if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            std::cout << "Present mode: Mainbox" << std::endl;
            return available_present_mode;
        }
    }

    std::cout << "Present mode: V-Sync (FIFO)" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

} // namespace renderer::detail