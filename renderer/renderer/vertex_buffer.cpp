#include <renderer/renderer/vertex_buffer.hpp>

// std
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace renderer {

VertexBuffer::VertexBuffer(Device& device, std::vector<Vertex> vertices, std::vector<uint16_t> indices)
    : device_{device}
{
    vertex_buffer_size_ = vertices.size();
    CreateVertexBuffer(std::move(vertices));

    if (!indices.empty())
    {
        has_indices_ = true;
        index_buffer_size_ = indices.size();
        CreateIndexBuffer(std::move(indices));
    }
}

VertexBuffer::~VertexBuffer()
{
    vkDestroyBuffer(device_.GetDevice(), vertex_buffer_, nullptr);
    vkFreeMemory(device_.GetDevice(), vertex_buffer_memory_, nullptr);
}

void VertexBuffer::DrawBuffer(VkCommandBuffer command_buffer)
{
    VkBuffer vertex_buffers[] = {vertex_buffer_};
    VkDeviceSize offsets[] = {0};
    
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

    if (has_indices_)
    {
        vkCmdBindIndexBuffer(command_buffer, index_buffer_, 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(index_buffer_size_), 1, 0, 0, 0);
    }
    else
    {
        vkCmdDraw(command_buffer, static_cast<uint32_t>(vertex_buffer_size_), 1, 0, 0);
    }
}


void VertexBuffer::CreateVertexBuffer(std::vector<Vertex> vertices)
{
    VkDeviceSize buffer_size = sizeof(std::vector<Vertex>::value_type) * vertices.size();   
    
    // Staging buffer step
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

    // copying vertices data into stagin data buffer
    void* data;
    vkMapMemory(device_.GetDevice(), staging_buffer_memory, 0, buffer_size, 0, &data);
    std::memcpy(data, vertices.data(), (size_t)buffer_size);
    vkUnmapMemory(device_.GetDevice(), staging_buffer_memory);

    // createing device local buffer
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertex_buffer_, vertex_buffer_memory_);

    CopyBuffer(staging_buffer, vertex_buffer_, buffer_size);

    vkDestroyBuffer(device_.GetDevice(), staging_buffer, nullptr);
    vkFreeMemory(device_.GetDevice(), staging_buffer_memory, nullptr);
}

void VertexBuffer::CreateIndexBuffer(std::vector<uint16_t> indices)
{
    VkDeviceSize buffer_size = sizeof(std::vector<uint16_t>::value_type) * indices.size();

    // Staging buffer step
    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

    // copying vertices data into stagin data buffer
    void* data;
    vkMapMemory(device_.GetDevice(), staging_buffer_memory, 0, buffer_size, 0, &data);
    std::memcpy(data, indices.data(), (size_t)buffer_size);
    vkUnmapMemory(device_.GetDevice(), staging_buffer_memory);

    // createing device local buffer
    CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, index_buffer_, index_buffer_memory_);

    CopyBuffer(staging_buffer, index_buffer_, buffer_size);

    vkDestroyBuffer(device_.GetDevice(), staging_buffer, nullptr);
    vkFreeMemory(device_.GetDevice(), staging_buffer_memory, nullptr);

}


void VertexBuffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory)
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device_.GetDevice(), &buffer_info, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create buffer!");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(device_.GetDevice(), buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device_.GetDevice(), &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device_.GetDevice(), buffer, buffer_memory, 0);

}


void VertexBuffer::CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = device_.GetCommandPool();
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(device_.GetDevice(), &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);
    {
        VkBufferCopy copy_region{};
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
    }
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(device_.GetGraphicsQueue(), 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(device_.GetGraphicsQueue());

    vkFreeCommandBuffers(device_.GetDevice(), device_.GetCommandPool(), 1, &command_buffer);
}

uint32_t VertexBuffer::FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
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