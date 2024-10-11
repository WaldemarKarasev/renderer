#include <renderer/app.hpp>

// std
#include <iostream>

// renderer includes
#include <renderer/renderer/buffer.hpp>


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
    ubo_buffers.resize(renderer::SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < ubo_buffers.size(); ++i)
    {
        ubo_buffers[i] = std::make_unique<renderer::Buffer>(device_, sizeof(renderer::UBO), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        ubo_buffers[i]->Map();
    }

    // Creating global descriptor layout
    global_descriptor_set_layout_ = renderer::DescriptorSetLayout::Builder(device_)
                    .AddBindings(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                    .Build();

    
    // allocating descriptor sets from global descriptor pool
    global_descriptor_sets_.resize(renderer::SwapChain::MAX_FRAMES_IN_FLIGHT);
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
            renderer::FrameInfo frame_info{};
            frame_info.frame_index_ = renderer_.GetFrameIndex();
            frame_info.command_buffer_ = command_buffer;
            frame_info.global_descriptor_set = global_descriptor_sets_[frame_info.frame_index_];

            UpdateUBO(frame_info);

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


void App::Render(const renderer::FrameInfo& frame_info)
{
    // rendering quad demo
    // bind pipeline
    vkCmdBindPipeline(frame_info.command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->GetGraphicsPipeline());

    // Binding descriptor sets
    vkCmdBindDescriptorSets(frame_info.command_buffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->GetLayout(), 0, 1, &frame_info.global_descriptor_set_, 0, nullptr);

    // draw cmd for quad vertex buffer
    vertex_buffer_->DrawBuffer(frame_info.command_buffer_);
}

void App::UpdateUBO(const renderer::FrameInfo& frame_info)
{
    static auto start_time = std::chrono::high_resolution_clock::now();

    auto current_time = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<float, std::chrono::seconds::period>(current_time - start_time).couunt();

    renderer::UBO ubo;
    ubo.model_ = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view_  = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); 
    ubo.proj_  = glm::perspective(glm::radiand(45.0f), renderer_.GetSwapChainExtent().width / static_cast<float>(renderer_.GetSwapChainExtent().height), 0.1f, 10.f);
    ubo.proj_[1][1] *= -1;

    ubo_buffers[frame_info.frame_index_]->WriteToBuffer(&ubo);
    ubo_buffers[frame_info.frame_index_]->Flush(); // TODO: try to comment this line when rendering
}



} // namespace engine