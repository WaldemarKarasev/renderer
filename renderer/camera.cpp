#include <renderer/camera.hpp>

namespace engine {

   
Camera::Camera(glm::vec3 position, glm::vec3 rotation, ProjectionMode mode)
    : position_(std::move(position))
    , rotation_(std::move(rotation))
    , mode_{mode}
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::SetPosition(glm::vec3 position)
{
    position_ = std::move(position);
    UpdateViewMatrix();
}

void Camera::SetRotation(glm::vec3 rotation)
{
    rotation_ = std::move(rotation);
    UpdateViewMatrix();

}

void Camera::SetPositionRotation(glm::vec3 position, glm::vec3 rotation)
{
    position_ = std::move(position);
    rotation_ = std::move(rotation);
    UpdateViewMatrix();
}

void Camera::SetProJectionMode(ProjectionMode mode)
{
    mode_ = mode;
    UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix()
{
    view_matrix_  = glm::lookAt(position_, rotation_, glm::vec3(.0f, -1.0f, .0f)); 
}

void Camera::UpdateProjectionMatrix()
{
    projection_matrix_ = glm::perspective(glm::radians(45.0f), 2.0f, 0.1f, 10.0f);
    projection_matrix_[1][1] *= -1;
}



} // namespace engine