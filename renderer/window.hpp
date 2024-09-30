#pragma once

// stl
#include <string>

// vulkan
#include <vulkan/vulkan.h>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace engine {

class Window
{
public:
    Window(std::string window_name, int width, int height);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // surface creation
    void CreateSurface(VkInstance instance, VkSurfaceKHR *surface);

    void GetFrameBufferSize(int* width, int* height);

private:
    void InitWindow();


    // resize callback
    static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);


private:
    int width_;
    int height_;
    std::string window_name_;
    bool framebuffer_resized_{false};

    GLFWwindow* window_{nullptr};
};

} // namesoace engine