#include <renderer/renderer/buffer.hpp>

// std
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <iostream>


namespace renderer {

Buffer::Buffer(Device& device,
        VkDeviceSize size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties)
        : device_{device}
        , size_{size}
{
    CreateBuffer(size, usage, properties);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(device_.GetDevice(), buffer_, nullptr);
    vkFreeMemory(device_.GetDevice(), memory_, nullptr);
}

VkResult Buffer::Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
{
    assert(mapped_ == nullptr);

    return vkMapMemory(device_.GetDevice(), memory_, offset, size, 0, &mapped_);
}

void Buffer::Unmap()
{
    assert(mapped_ != nullptr);
    vkUnmapMemory(device_.GetDevice(), memory_);
    mapped_ = nullptr;
}

void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset = 0)
{
    assert(mapped_ != nullptr);

    if (size == VK_WHOLE_SIZE)
    {
        std::memcpy(mapped_, data, size_);
    }
    else
    {
        // Not implemented
    }
}

void Buffer::CopyBuffers(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{

}

void Buffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device_.GetDevice(), &buffer_info, nullptr, &buffer_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create buffer!");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device_.GetDevice(), buffer_, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device_.GetDevice(), &alloc_info, nullptr, &memory_) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device_.GetDevice(), buffer_, memory_, 0);
}

uint32_t Buffer::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(device_.GetPhysicalDevice(), &mem_properties);
    
    std::cout << "mem_properties.memoryTypeCount=" << mem_properties.memoryTypeCount << std::endl;
    std::cout << "type_filter=" << type_filter << std::endl;
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i)
    {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    
    throw std::runtime_error("Failed to find suitable memory type!");
}



} // namespace renderer