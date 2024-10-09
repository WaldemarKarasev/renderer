#include <renderer/app.hpp>

// std
#include <iostream>

// renderer includes
#include <renderer/renderer/buffer.hpp>
#include <renderer/renderer/frame_utility.hpp>


namespace engine {

struct UBO
{

};

App::App()
{
    // Creating pipeline for quad rendering
    pipeline_ = std::make_unique<Pipeline>(device_, swap_chain_.GetRenderPass(), global_layout_.GetLayout());


    // Initializing vertex buffer for quad
    std::vector<Vertex> square = {           /* Vertices */
                                    {{-0.5f, -0.5f},    {1.0f, 0.0f, 0.0f}},
                                    {{0.5f, -0.5f},     {0.0f, 1.0f, 0.0f}},
                                    {{0.5f, 0.5f},      {0.0f, 0.0f, 1.0f}},
                                    {{-0.5f, 0.5f},     {1.0f, 1.0f, 1.0f}}
                                };
                                        /* Indices */
    std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    vertex_buffer_ = std::make_unique<VertexBuffer>(device_, std::move(square), std::move(indices));
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

    // global descriptor pool

    // allocating descriptor sets from global descriptor pool

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
    // draw cmd for quad vertex buffer
    vertex_buffer_->DrawBuffer(command_buffer);
}

} // namespace engine