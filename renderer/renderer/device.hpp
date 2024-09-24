#pragma once

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

namespace renderer {

class Device
{
public:
    Device();
    ~Device();
private:
    void InitVulkan();

    // Instance
    void CreateInstance();
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

private:
    VkInstance instance_;

private:
    inline static const std::vector<const char*> s_validation_layers_ = {
        "VK_LAYER_KHRONOS_validation"
    };
};

} // namespace renderer