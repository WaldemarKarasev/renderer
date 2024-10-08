#pragma once

// std
#include <optional>
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

namespace renderer::detail {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family_;
    std::optional<uint32_t> present_family_;

    bool IsComplete() 
    {
        return graphics_family_.has_value() && present_family_.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities_;
    std::vector<VkSurfaceFormatKHR> formats_;
    std::vector<VkPresentModeKHR> present_modes_;
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

class PhysicalDeviceSelector
{
public:
    PhysicalDeviceSelector(std::vector<VkPhysicalDevice>& devices, VkSurfaceKHR& surface)
        : devices_{devices}
        , surface_{surface}
    {}

    VkPhysicalDevice Select();
private:
    bool IsDeviceSuitable(VkPhysicalDevice device);

private:
    std::vector<VkPhysicalDevice>& devices_;
    VkSurfaceKHR& surface_;
};

} // namespace renderer::detail