#pragma once

// vulkan
#include <vulkan/vulkan.h>

// glm
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace renderer
{
    
    struct UBO
    {
        glm::mat4 model_;
        glm::mat4 view_;
        glm::mat4 proj_;
    };

    struct FrameInfo
    {
        int frame_index_;
        VkCommandBuffer command_buffer_;
        VkDescriptorSet global_descriptor_set_;
    };


} // namespace renderer
