#pragma once

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

// engine includes
#include <renderer/window.hpp>

namespace renderer {

class Device
{
public:
    Device(engine::Window& window);
    ~Device();
private:
    void InitVulkan();

    // Instance
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommandPools();

private:
    // window for rendering to it
    engine::Window& window_;

    VkInstance instance_;
    VkDebugUtilsMessengerEXT debug_messenger_;

    // surface
    VkSurfaceKHR surface_;

    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    VkDevice device_;

    // device_ queues
    VkQueue graphics_queue_;
    VkQueue present_queue_;

    // devide_ command pool
    VkCommandPool command_pool_;
};

} // namespace renderer