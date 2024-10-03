#include <renderer/window.hpp>

// std
#include <iostream>

namespace engine {

Window::Window(std::string name, int width, int height)
    : width_(width)
    , height_(height)
    , window_name_(std::move(name))
{
    InitWindow();
}
    
Window::~Window()
{

}

void Window::InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window_ = glfwCreateWindow(width_, height_, window_name_.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, FrameBufferResizeCallback);
    std::cout << "Window init finished" << std::endl;
}

// surface creation
void Window::CreateSurface(VkInstance instance, VkSurfaceKHR *surface)
{
    if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS)
    {
        std::cerr << "Failed to create Vulkan Window surface" << std::endl;
        std::abort();
    }
}

void Window::GetFrameBufferSize(int* width, int* height)
{
    glfwGetFramebufferSize(window_, width, height);
}

VkExtent2D Window::GetExtent()
{
    return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)};
}


void Window::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto window_impl = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    window_impl->framebuffer_resized_ = true;
    window_impl->width_ = width;
    window_impl->height_ = height;
}

} // namespace engine