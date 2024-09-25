#pragma once

// c-include
#include <cstring>

// std
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

namespace renderer::detail {

    static const std::vector<const char*> s_validation_layers_ = {
        "VK_LAYER_KHRONOS_validation"
    };

    inline bool CheckValidationLayerSupport()
    {
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        for (const char* layer_name : s_validation_layers_)
        {
            bool layer_found = false;

            for (const auto& layer_properties : available_layers)
            {
                if (std::strcmp(layer_name, layer_properties.layerName) == 0)
                {
                    layer_found = true;
                }
            }

            if (!layer_found)
            {
                return false;
            }
        }

        return true;
    }


} // namespace renderer::detail