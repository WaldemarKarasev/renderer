#include <renderer/input/input.hpp>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace systems
{

bool GLFWInput::IsKeyPressed(int keycode)
{
    auto state = glfwGetKey(window_, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool GLFWInput::IsMouseButtonPressed(int button)
{
    auto state = glfwGetMouseButton(window_, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> GetMousePosition()
{
    float x_pos;
    float y_pos;

    glfwGetCursorPos(window_, &x_pos, &y_pos);

    return {x_pos, y_pos};
}

float GLFWInput::GetMouseX()
{
    auto[x, y] = GetMousePosition();
    return x;
}

float GLFWInput::GetMouseY()
{
    auto[x, y] = GetMousePosition();
    return y;
}

} // namespace systems

