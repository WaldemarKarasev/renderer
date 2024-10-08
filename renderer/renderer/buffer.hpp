#pragma once

// std

// vulkan
#include <vulkan/vulkan.h>

// renderer includes
#include <renderer/renderer/device.hpp>


namespace renderer {

class Buffer
{
public:
    Buffer(Device& device,
           VkDeviceSize size, 
           VkBufferUsageFlags usage, 
           VkMemoryPropertyFlags properties);

    ~Buffer();

    VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void Unmap();
    void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    static void CopyBuffers(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

private:
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

private:
    Device& device_;

    VkBuffer buffer_;
    VkDeviceMemory memory_;
    VkDeviceSize size_;

    void* mapped_ = nullptr;
};

} // namespace renderer