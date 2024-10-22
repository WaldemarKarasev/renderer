#pragma once

// std
#include <iostream>

// glm
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

// camera
#include <renderer/camera.hpp>
#include <renderer/input/input.hpp>

namespace engine
{
class CameraController
{
public:
    void MoveCammera(Camera& camera, systems::GLFWInput& input)
    {
        glm::vec3 rotation_delta{0.0f};
        glm::vec3 position_delta{0.0f};

        if (input.IsKeyPressed(systems::Key::RIGHT)) { rotation_delta.y += s_rotation_speed_; }
        if (input.IsKeyPressed(systems::Key::LEFT))  { rotation_delta.y -= s_rotation_speed_; }
        if (input.IsKeyPressed(systems::Key::UP))    { rotation_delta.x += s_rotation_speed_;}
        if (input.IsKeyPressed(systems::Key::DOWN))  { rotation_delta.x -= s_rotation_speed_; }
        
        camera.SetRotation(camera.GetRotation() + rotation_delta);

        auto current_rotation = camera.GetRotation();
        float yaw = current_rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
        const glm::vec3 upDir{0.f, -1.f, 0.f};

        glm::vec3 moveDir{0.0f};
        if (input.IsKeyPressed(systems::Key::W)) { moveDir += forwardDir; }//position_delta.z -= s_move_speed_; }
        if (input.IsKeyPressed(systems::Key::S)) { moveDir -= forwardDir; }//position_delta.z += s_move_speed_; }
        if (input.IsKeyPressed(systems::Key::A)) { moveDir += rightDir; }//position_delta.x -= s_move_speed_;}
        if (input.IsKeyPressed(systems::Key::D)) { moveDir -= rightDir; }//position_delta.x += s_move_speed_; }
        if (input.IsKeyPressed(systems::Key::Q)) { moveDir += upDir; }//position_delta.y -= s_move_speed_; }
        if (input.IsKeyPressed(systems::Key::E)) { moveDir -= upDir; }//position_delta.y += s_move_speed_; }
        

        std::cout << "rotation_delta.x=" << rotation_delta.x   << 
                     ", rotation_delta.y=" << rotation_delta.y << 
                     ", rotation_delta.z=" << rotation_delta.z << std::endl;

        std::cout << "moveDir.x=" << moveDir.x   << 
                     ", moveDir.y=" << moveDir.y << 
                     ", moveDir.z=" << moveDir.z << std::endl;
        camera.SetPosition(camera.GetPosition() + moveDir * s_move_speed_);

    }
private:
    static constexpr float s_move_speed_ = 0.0005f;
    static constexpr float s_rotation_speed_ = 0.0005f;
};

} // namespace engine
