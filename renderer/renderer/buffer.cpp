#include <renderer/renderer/buffer.hpp>

// std
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <iostream>


namespace renderer {

Buffer::Buffer(Device& device,
        VkDeviceSize instance_size, 
        uint32_t instance_count,
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties)
        : device_{device}
        , instance_size_{instance_size}
        , instance_count_{instance_count}
{
    buffer_size_ = instance_count_ * instance_size_;
    CreateBuffer(buffer_size_, usage, properties);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(device_.GetDevice(), buffer_, nullptr);
    vkFreeMemory(device_.GetDevice(), memory_, nullptr);
}

VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset)
{
    return vkMapMemory(device_.GetDevice(), memory_, offset, size, 0, &mapped_);
}

void Buffer::Unmap()
{
    vkUnmapMemory(device_.GetDevice(), memory_);
    mapped_ = nullptr;
}

void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
{
    assert(mapped_ && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE)
    {
        std::memcpy(mapped_, data, buffer_size_);
    }
    else
    {
        char* mem_offset = reinterpret_cast<char*>(mapped_);
        mem_offset += offset;
        std::memcpy(mem_offset, data, size);
    }
}

VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset)
{
    VkMappedMemoryRange mapped_range = {};
    mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mapped_range.memory = memory_;
    mapped_range.offset = offset;
    mapped_range.size = size;

    return vkFlushMappedMemoryRanges(device_.GetDevice(), 1, &mapped_range);
}

VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
{
    return VkDescriptorBufferInfo{
        buffer_,
        offset,
        size
    };
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