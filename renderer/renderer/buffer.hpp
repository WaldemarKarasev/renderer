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
           VkDeviceSize instance_size,
           uint32_t  instance_count,
           VkBufferUsageFlags usage, 
           VkMemoryPropertyFlags properties);

    ~Buffer();

    VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void Unmap();
    void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    // Getters

    VkBuffer GetBuffer() { return buffer_; }
    uint32_t GetInstanceCount() { return instance_count_; }

private:
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

private:
    Device& device_;

    VkBuffer buffer_;
    VkDeviceMemory memory_;
    VkDeviceSize buffer_size_;
    VkDeviceSize instance_size_;
    uint32_t instance_count_;


    void* mapped_ = nullptr;
};

} // namespace renderer