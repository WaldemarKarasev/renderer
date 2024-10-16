#pragma once

// std
#include <array>
#include <memory>
#include <vector>

// Vulkan
#include <vulkan/vulkan.h>

// glm
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

// renderer includes
#include <renderer/renderer/device.hpp>
#include <renderer/renderer/buffer.hpp>


namespace renderer {

struct Vertex 
{
    glm::vec3 pos_;
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
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
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

private:
    Device& device_;

    std::unique_ptr<Buffer> vertex_buffer_ = nullptr;
    size_t vertex_buffer_size_{0};

    bool has_indices_ = false;
    std::unique_ptr<Buffer> index_buffer_ = nullptr;
    size_t index_buffer_size_{0};
};

} // namespace renderer