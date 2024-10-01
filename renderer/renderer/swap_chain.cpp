#include <renderer/renderer/swap_chain.hpp>

// std
#include <stdexcept>

// utils includes
#include <renderer/renderer/detail/physical_device_utils.hpp>
#include <renderer/renderer/detail/debug_utils.hpp>



namespace renderer {

SwapChain::SwapChain(engine::Window& window, Device& device)
    : window_{window}
    , device_{device}
{
    CreateSwapChain();
    CreateSwapChainImageViews();
    CreateRenderPass();
    CreateFramebuffers();
    CreateSyncObjects();
}

SwapChain::~SwapChain()
{
    for (auto image_view : swap_chain_image_views_) 
    {
        vkDestroyImageView(device_.GetDevice(), image_view, nullptr);
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device_.GetDevice(), render_finished_semaphores_[i], nullptr);
        vkDestroySemaphore(device_.GetDevice(), image_available_semaphores_[i], nullptr);
        vkDestroyFence(device_.GetDevice(), in_flight_fences_[i], nullptr);
    }

    vkDestroySwapchainKHR(device_.GetDevice(), swap_chain_, nullptr);

    vkDestroyRenderPass(device_.GetDevice(), render_pass_, nullptr);    
}

VkCommandBuffer SwapChain::BeginFrame(uint32_t* image_index)
{
    // begin frame
    vkWaitForFences(device_.GetDevice(), 1, &in_flight_fences_[current_frame_], VK_TRUE, UINT64_MAX);

    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(device_.GetDevice(), swap_chain_, UINT64_MAX, image_available_semaphores_[current_frame_], VK_NULL_HANDLE, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return nullptr;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    vkResetFences(device_.GetDevice(), 1, &in_flight_fences_[current_frame_]);
    vkResetCommandBuffer(command_buffers_[current_frame_]);

    // begin render pass
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffers_[current_frame_], &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return command_buffers_[current_frame_];
}
void SwapChain::EndFrame(VkCommandBuffer command_buffer, uint32_t image_index)
{
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }    

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {image_available_semaphores_[current_frame_]};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    VkSemaphore signal_semaphores[] = {render_finished_semaphores_[current_frame_]};

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    if (vkQueueSubmit(device_.GetGraphicsQueue(), 1, &submit_info, in_flight_fences_[current_frame_]) != VK_SUCCESS) {        
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swap_chains[] = {swap_chain_};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;

    present_info.pImageIndices = image_index;

    result = vkQueuePresentKHR(device_.GetPresentQueue(), &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebuffer_resized_) {
        framebuffer_resized_ = false;
        RecreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    current_frame_ = (current_frame_ + 1) % MAX_FRAMES_IN_FLIGHT;
}


void SwapChain::BeginRenderPass(VkCommadBuffer command_buffer, uint32_t* image_index)
{
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = render_pass_;
    render_pass_info.framebuffer = swap_chain_framebuffers_[*image_index];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swap_chain_extent_;

    VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)  swap_chain_extent_.width;
    viewport.height = (float) swap_chain_extent_.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(command_buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swap_chain_extent_;
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
}

void SwapChain::EndRenderPass(VkCommandBuffer command_buffer)
{
    // end render pass
    vkCmdEndRenderPass(command_buffer);
}
















void SwapChain::CreateSwapChain()
{
    detail::SwapChainSupportDetails swap_chain_support = detail::QuerySwapChainSupport(device_.GetPhysicalDevice(), device_.GetSurface());

    VkSurfaceFormatKHR surface_format = detail::ChooseSwapSurfaceFormat(swap_chain_support.formats_);
    VkPresentModeKHR present_mode = detail::ChooseSwapPresentMode(swap_chain_support.present_modes_);
    VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities_)

    uint32_t image_count = swap_chain_support.capabilities_.minImageCount + 1;
    if (swap_chain_support.capabilities_.maxImageCount > 9 && image_count > swap_chain_support.capabilities_.maxImageCount)
    {
        image_count = swap_chain_support.capabilities_.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = device_.GetSurface();

    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    detail::QueueFamilyIndices indices = detail::FindQueueFamilies(device_.GetPhysicalDevice(), device_.GetSurface());
    uint32_t queue_family_indices[] = {indices.graphics_family_.value(), indices.present_family_.value()}:

    if (indices.graphics_family_ != indices.present_family_)
    {
        create_info.imangeSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform = swap_chain_support.capabilities_.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = preset_mode;
    create_info.clipped = VK_TRUE;

    if (vkCreateSwapChainKHR(device_.GetDevice(), &create_info, nullptr, &swap_chain_) != VK_SUCCESS)
    {
        std::cerr << "Failed to create Vulkan swap chain!" << std::endl;
        std::abort();
    }

    vkCreateSwapChainImageKHR(device_.GetDevice(), swap_chain_, &image_count, nullptr);
    swap_chain_images_.resize(image_count);
    vkGetSwapChainImageKHR(device_.GetDevice(), swap_chain_, &image_count, swap_chain_images_.data());

    swap_chain_image_format_ = surface_format_.format;
    swap_chain_extent_ = extent;
}   

void SwapChain::CreateSwapChainImageViews()
{
    swap_chain_image_views_.resize(swap_chain_images_.size());

    for (size_t i = 0; i < swap_chain_image_views_.size(); ++i)
    {
        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_FORMAT_INFO;
        crete_info.image = swap_chain_images_[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = swap_chain_image_format_;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device_.GetDevice(), &create_info, nullptr, &swap_chain_image_views_[i]) != VK_SUCCESS)
        {
            std::cerr << "Failed to create image views!" << std::endl;
            std::abort();
        } 
    }
}


VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width;
        int height;

        glfwGetFramebufferSize(window_, &width, &height);
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void SwapChain::CreateFramebuffers()
{
    auto image_views = swap_chain_.GetImageViews();
    swap_chain_framebuffers_.resize(image_views.size());

    for (size_t i = 0; i < image_views.size(); ++i)
    {
        VkImageView attachments[] = {image_views[i]};

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = renderPass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = swapChainExtent.width;
        framebuffer_info.height = swapChainExtent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFrameBuffer(device_.GetDevice(), &frame_buffer_info, nullptr, &swap_chain_framebuffers_[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}

void SwapChain::CreateSyncObjects()
{
    image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &image_available_semaphores_[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device_.GetDevice(), &semaphore_info, nullptr, &render_finished_semaphores_[i]) != VK_SUCCESS ||
            vkCreateFence(device_.GetDevice(), &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}


void SwapChain::RecreateSwapChain()
{
    int width = 0;
    int height = 0;

    while (width == 0 || height == 0)
    {
        window_.GetFrameBufferSize(&width, &height);
    }

    vkDeviceWaitIdle(device_.GetDevice());
    
    CleanupSwapChain();

    CreateSwapChain();
    CreateSwapChainImages();
    CreateFramebuffers();
}

void SwapChain::CreateRenderPass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swap_chain_.GetImageFormat();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachmetnt.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &colorAttachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(device_.GetDevice(), &render_pass_info, nullptr, &render_pass_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }    
}

} // namespace renderer