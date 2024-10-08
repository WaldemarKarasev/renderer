#include <renderer/app.hpp>

// std
#include <iostream>

// renderer includes
#include <renderer/renderer/buffer.hpp>
#include <renderer/renderer/frame_utility.hpp>


#include <chrono>

namespace engine {

App::App()
{

    // Initializing vertex buffer for quad
    std::vector<renderer::Vertex> square = {           /* Vertices */
                                    {{-0.5f, -0.5f},    {1.0f, 0.0f, 0.0f}},
                                    {{0.5f, -0.5f},     {0.0f, 1.0f, 0.0f}},
                                    {{0.5f, 0.5f},      {0.0f, 0.0f, 1.0f}},
                                    {{-0.5f, 0.5f},     {1.0f, 1.0f, 1.0f}}
                                };
                                        /* Indices */
    std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    vertex_buffer_ = std::make_unique<renderer::VertexBuffer>(device_, std::move(square), std::move(indices));

    // global descriptor pool
    global_descriptor_pool_ = 
        renderer::DescriptorPool::Builder(device_)
        .SetMaxSets(renderer::SwapChain::MAX_FRAMES_IN_FLIGHT)
        .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, renderer::SwapChain::MAX_FRAMES_IN_FLIGHT)
        .Build();
}

void App::Run()
{   
    // =========================================== Preparation for begining main loop =========================================== //

    // Creating uniform buffers for future its usage in descriptor sets
    std::vector<std::unique_ptr<renderer::Buffer>> ubo_buffers(renderer::SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < ubo_buffers.size(); ++i)
    {
        ubo_buffers[i] = std::make_unique<renderer::Buffer>(device_, sizeof(renderer::UBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        ubo_buffers[i]->Map();
    }

    // Creating global descriptor layout
    auto global_descriptor_set_layout_ = renderer::DescriptorSetLayout::Builder(device_)
                    .AddBindings(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                    .Build();

    
    // allocating descriptor sets from global descriptor pool
    std::vector<VkDescriptorSet> global_descriptor_sets(renderer::SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < global_descriptor_sets.size(); ++i)
    {
        auto buffer_info = ubo_buffers[i]->DescriptorInfo();
        renderer::DescriptorWriter(*global_descriptor_set_layout_, *global_descriptor_pool_)
            .WriteBuffer(0, &buffer_info)
            .Build(global_descriptor_sets[i]);
    }
    
    // Creating pipeline for quad rendering. For now this is a kind of prototype for the rendering system 
    pipeline_ = std::make_unique<renderer::Pipeline>(device_, renderer_.GetSwapchainRenderPass(), global_descriptor_set_layout_->GetDescriptorSetLayout());

    while(!window_.ShouldClose())
    {
        // main loop
        glfwPollEvents();
        
        // std::cout << "loop" << std::endl;
        if (auto command_buffer = renderer_.BeginFrame())
        {
            renderer_.BeginRenderPass(command_buffer);
            Render(command_buffer);
            renderer_.EndRenderPass(command_buffer);
            renderer_.EndFrame(command_buffer);
        }
        else
        {
            std::cout << "Error receiving command_buffer from BeginFrame function" << std::endl;
        }
    }
}


void App::Render(VkCommandBuffer command_buffer)
{
    // rendering quad demo
    // bind pipeline
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->GetGraphicsPipeline());

    // Binding descriptor sets
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->GetLayout(), 0, 1, nullptr, 0, nullptr);

    // draw cmd for quad vertex buffer
    vertex_buffer_->DrawBuffer(command_buffer);
}

void App::UpdateUniformBuffer(uint32_t current_image)
{
    #if 0
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    renderer::UBO ubo{};
    ubo.model_ = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view_ = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj_ = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);
    ubo.proj_[1][1] *= -1;

    std::memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));   
    #endif
}


} // namespace engine