#include <renderer/input/input.hpp>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace engine::systems {

void GLFWInput::Update()
{
    for (const auto& [key, value] : pressed_buttons_)
    {
        auto state = glfwGetKey(window_.GetNativeWindow(), static_cast<int>(key));
        if (state == GLFW_PRESS || state == GLFW_REPEAT)
        {
            pressed_buttons_[key] = true;
        }
        else
        {
            pressed_buttons_[key] = false;
        }
    }
}

bool GLFWInput::IsKeyPressed(Key keycode)
{
    if (pressed_buttons_.count(keycode))
    {
        return pressed_buttons_[keycode];
    }

    return false;
}

bool GLFWInput::IsMouseButtonPressed(Key button)
{
    // Not implemented
    return false;
}

std::pair<float, float> GLFWInput::GetMousePosition()
{
    // Not implemented
    return {};
}

float GLFWInput::GetMouseX()
{
    // Not implemented
    return {};
}

float GLFWInput::GetMouseY()
{
    // Not implemented
    return {};
}

} // namespace systems

