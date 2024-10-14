#pragma once

#include <renderer/window.hpp>


namespace systems {

class GLFWInput
{
public:
    GLFWInput(engine::Window& window) : window_{window};
    bool IsKeyPressed(int keycode);
    bool IsMouseButtonPressed(int button);
    std::pair<float, float> GetMousePosition();
    float GetMouseX();
    float GetMouseY();

private:
    engine::Window& window_;
};

} // namespace systems