#pragma once

// glm
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace engine
{
    
class Camera
{
public:
    enum class ProjectionMode
    {
        Perspective,
        Orthographic
    };

public:
    Camera(glm::vec3 position = {0, 0, 0}, glm::vec3 rotation = {0, 0, 0}, ProjectionMode mode = ProjectionMode::Perspective);

    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetPositionRotation(glm::vec3 position, glm::vec3 rotation);
    void SetProJectionMode(ProjectionMode mode);

    glm::mat4x4 GetViewMatrix() { return view_matrix_; }
    glm::mat4x4 GetProjMatrix() { return projection_matrix_; }

    glm::vec3 GetPosition() { return position_; }
    glm::vec3 GetRotation() { return rotation_; }

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

private:
    glm::vec3 position_;
    glm::vec3 rotation_;
    ProjectionMode mode_;
    glm::mat4x4 view_matrix_;
    glm::mat4x4 projection_matrix_;
};

} // namespace engine
