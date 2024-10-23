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

    VkDevice GetDevice() { return device_; }
    VkPhysicalDevice GetPhysicalDevice() { return physical_device_; }
    VkSurfaceKHR GetSurface()            { return surface_; }

    VkQueue GetGraphicsQueue() { return graphics_queue_; }
    VkQueue GetPresentQueue() { return present_queue_; }

    VkCommandPool GetCommandPool() { return command_pool_; }


private:
    void InitVulkan();

    // Instance
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateCommapdPool();

public:
    // buffer utility interface
    void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates
                                , VkImageTiling tilling
                                , VkFormatFeatureFlags features);
    
    void CreateImageWithInfo(const VkImageCreateInfo &image_info
                            , VkMemoryPropertyFlags properties
                            , VkImage &image
                            , VkDeviceMemory &image_memory);
    uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);
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

    // command pool
    VkCommandPool command_pool_;

};

} // namespace renderer