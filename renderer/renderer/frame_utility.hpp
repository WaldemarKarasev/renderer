#pragma once

// glm
#include <glm/mat4.hpp>

namespace renderer
{
    
    struct UBO
    {
        glm::mat4 proj_;
        glm::mat4 model_;
        glm::mat4 view_;
    };

} // namespace renderer
