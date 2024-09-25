#include <physical_device_utils.hpp>

// std
#include <optional>
#include <string>

namespace renderer::detail {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family_;
    std::optional<uint32_t> present_family_;

    bool IsComplete() 
    {
        return graphics_family.has_value() && present_family_.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities_;
    std::vector<VkSurfaceFormatKHR> formats_;
    std::vector<VkPresentModeKHR> present_modes_;
};

static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    int i = 0;
    for (const auto& queue_family : queue_families) 
    {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            indices.graphics_family_ = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);

        if (present_support) 
        {
            indices.present_family_ = i;
        }

        if (indices.IsComplete()) 
        {
            break;
        }

        ++i;
    }

    return indices;
}


bool CheckDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities_);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

    if (format_count != 0) 
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

    if (present_mode_count != 0) 
    {
        details.presentModes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.presentModes.data());
    }

    return details;
}

bool IsDeviceSuitable(VkPhysicalDevice physical_device)
{
    QueueFamilyIndices indices = FindQueueFamilies(physical_device);

    bool extensions_supported = CheckDeviceExtensionSupport(physical_device);

    bool swap_chain_adequate = false;
    
    if (extensions_supported)
    {
        SwapChainSupportDetails spaw_chain_support = QuerySwapChainSupport(physical_device);
        swap_chain_adequate = !spaw_chain_support.formats_.empty() && !spaw_chain_support.present_modes.empty();
    }

    return indices.IsComplete() && extensions_supported && swap_chain_adequate;
}

} // namespace renderer::detail