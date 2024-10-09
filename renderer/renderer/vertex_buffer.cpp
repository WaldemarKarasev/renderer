#include <renderer/renderer/vertex_buffer.hpp>

// std
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace renderer {

VertexBuffer::VertexBuffer(Device& device, std::vector<Vertex> vertices, std::vector<uint16_t> indices)
    : device_{device}
{
    CreateVertexBuffer(std::move(vertices));
    vertex_buffer_size_ = vertex_buffer_->GetInstanceCount();

    if (!indices.empty())
    {
        has_indices_ = true;
        index_buffer_size_ = indices.size();
        CreateIndexBuffer(std::move(indices));
        index_buffer_size_ = index_buffer_->GetInstanceCount();
    }
}

VertexBuffer::~VertexBuffer()
{
}

void VertexBuffer::DrawBuffer(VkCommandBuffer command_buffer)
{
    VkBuffer vertex_buffers[] = {vertex_buffer_->GetBuffer()};
    VkDeviceSize offsets[] = {0};
    
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);

    if (has_indices_)
    {
        vkCmdBindIndexBuffer(command_buffer, index_buffer_->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
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
    
    // Staging buffer setup
    auto staging_buffer = std::make_unique<Buffer>(device_, sizeof(Vertex), vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    // copying vertices data into stagin data buffer
    staging_buffer->Map();
    staging_buffer->WriteToBuffer(vertices.data());
    staging_buffer->Unmap();

    // creating device local vertex buffer
    vertex_buffer_ = std::make_unique<Buffer>(device_, sizeof(Vertex), vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

// copying from host visible to visible only for device memory
    device_.CopyBuffer(staging_buffer->GetBuffer(), vertex_buffer_->GetBuffer(), buffer_size);
}

void VertexBuffer::CreateIndexBuffer(std::vector<uint16_t> indices)
{
    VkDeviceSize buffer_size = sizeof(std::vector<uint16_t>::value_type) * indices.size();

    // Staging buffer step
    auto staging_buffer = std::make_unique<Buffer>(device_, sizeof(Vertex), indices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // copying vertices data into stagin data buffer
    staging_buffer->Map();
    staging_buffer->WriteToBuffer(indices.data());
    staging_buffer->Unmap();

    // createing device local index buffer
    index_buffer_ = std::make_unique<Buffer>(device_, sizeof(Vertex), indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // copying from host visible to visible only for device memory
    device_.CopyBuffer(staging_buffer->GetBuffer(), index_buffer_->GetBuffer(), buffer_size);
}

} // namespace renderer