#pragma once

// std
#include <array>
#include <vector>

// Vulkan
#include <vulkan/vulkan.h>

// glm
#include <glm/glm.hpp>

// renderer includes
#include <renderer/renderer/device.hpp>



namespace renderer {

struct Vertex 
{
    glm::vec2 pos_;
    glm::vec3 col_;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{};
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, pos_);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, col_);

        return attribute_descriptions;
    }
};


class VertexBuffer
{
public:
    VertexBuffer(Device& device, std::vector<Vertex> vertices, std::vector<uint16_t> indices = {});
    ~VertexBuffer();

    void DrawBuffer(VkCommandBuffer command_buffer);

private:
    void CreateVertexBuffer(std::vector<Vertex> vertices);
    void CreateIndexBuffer(std::vector<uint16_t> indices);

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);
    void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

    uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

private:
    Device& device_;

    VkBuffer vertex_buffer_;
    VkDeviceMemory vertex_buffer_memory_;
    size_t vertex_buffer_size_{0};

    bool has_indices_ = false;
    VkBuffer index_buffer_;
    VkDeviceMemory index_buffer_memory_;
    size_t index_buffer_size_{0};
};

} // namespace renderer