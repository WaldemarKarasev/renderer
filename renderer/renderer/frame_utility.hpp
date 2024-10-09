#pragma once

// glm
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace renderer
{
    
    struct UBO
    {
        glm::mat4 proj_;
        glm::mat4 model_;
        glm::mat4 view_;
    };

} // namespace renderer
