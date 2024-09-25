#pragma once

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace renderer::detail {

    extern const std::vector<const char*> glob_validationLayers;

    inline std::vector<const char*> GetRequiredExtensions()
    {
        uint32_t glfw_extensions_count = 0;
        const char** glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);

        std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extensions_count);

        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

} // namespace renderer::detail