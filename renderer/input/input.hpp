#pragma once

// std
#include <unordered_map>

// key codes
#include <renderer/input/key_codes.hpp>

// window impl
#include <renderer/window.hpp>

namespace engine::systems {

class GLFWInput
{
public:
    struct KeyMappings
    {
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_E;
        int moveDown = GLFW_KEY_Q;
        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;
    };


public:
    GLFWInput(engine::Window& window) : window_{window}
    {
        pressed_buttons_[Key::A]        = false;
        pressed_buttons_[Key::D]        = false;
        pressed_buttons_[Key::W]        = false;
        pressed_buttons_[Key::S]        = false;
        pressed_buttons_[Key::E]        = false;
        pressed_buttons_[Key::Q]        = false;
        pressed_buttons_[Key::LEFT]     = false;
        pressed_buttons_[Key::RIGHT]    = false;
        pressed_buttons_[Key::UP]       = false;
        pressed_buttons_[Key::DOWN]     = false;
    }

    void Update();
    bool IsKeyPressed(Key keycode);

private:
    bool IsMouseButtonPressed(Key button);
    std::pair<float, float> GetMousePosition();
    float GetMouseX();
    float GetMouseY();

private:
    engine::Window& window_;

    // for currrent implementation
    std::unordered_map<Key, bool> pressed_buttons_;

};

} // namespace engine::systems