#include <renderer/renderer/detail/physical_device_utils.hpp>
// std
#include <optional>
#include <string>
#include <vector>
#include <set>

namespace renderer::detail {

static const std::vector<const char*> s_device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities_;
    std::vector<VkSurfaceFormatKHR> formats_;
    std::vector<VkPresentModeKHR> present_modes_;
};

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
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
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

    std::set<std::string> required_extensions(s_device_extensions.begin(), s_device_extensions.end());

    for (const auto& extension : available_extensions) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities_);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

    if (format_count != 0) 
    {
        details.formats_.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats_.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

    if (present_mode_count != 0) 
    {
        details.present_modes_.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes_.data());
    }

    return details;
}


VkPhysicalDevice PhysicalDeviceSelector::Select() 
{
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    for (const auto& device : devices_)
    {
        if (IsDeviceSuitable(device))
        {
            physical_device = device;
            break;
        }
    }


    return physical_device;
}

bool PhysicalDeviceSelector::IsDeviceSuitable(VkPhysicalDevice physical_device)
{
    QueueFamilyIndices indices = FindQueueFamilies(physical_device, surface_);

    bool extensions_supported = CheckDeviceExtensionSupport(physical_device);

    bool swap_chain_adequate = false;
    
    if (extensions_supported)
    {
        SwapChainSupportDetails spaw_chain_support = QuerySwapChainSupport(physical_device, surface_);
        swap_chain_adequate = !spaw_chain_support.formats_.empty() && !spaw_chain_support.present_modes_.empty();
    }

    return indices.IsComplete() && extensions_supported && swap_chain_adequate;
}

} // namespace renderer::detail