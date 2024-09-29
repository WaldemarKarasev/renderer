#include <renderer/renderer/pipeline.hpp>

// std
#include <fstream>
#include <iostream>

namespace renderer {

Pipeline::Pipeline()
{
    CreatePipeline();
}

Pipeline::~Pipeline()
{

}

void Pipeline::CeatePipeline()
{
    auto vert_shader_code = ReadFile("shaders/vert.spv");
    auto frag_shader_code = ReadFile("shader/frag.spv");

    VkShaderModule vert_shader_module = CreateShaderModule(vert_shader_code);
    VkShaderModule frag_shader_module = CreateShaderModule(frag_shader_code);

}

std::vector<char> Pipeline::ReadFile(std::string filname)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " + filename << std::endl;
        std::abort(); 
    }

    size_t file_size = file.tellg();
    std::vector<char> code(file_size);

    file.seekg(0);
    file.read(code.data(), file_size);

    return code;
}

VkShaderModule Pipeline::CreateShaderModule(std::vector<char>& code)
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shader_module;

    if (vkCreateShaderModule(device_.GetDevice(), &create_info, nullptr, &shader_module) != VK_SUCCESS) 
    {
        std::cerr << "Failed to create shader module" << std::endl;
        std::abort();
    }

    return shader_module;
}

} // namespace renderer