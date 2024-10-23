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
    const float roll_in_radians  = glm::radians(rotation_.x);
    const float pitch_in_radians = glm::radians(rotation_.y);
    const float yaw_in_radians   = glm::radians(rotation_.z);

    const glm::mat3 rotate_matrix_x(1, 0, 0,
        0, cos(roll_in_radians), sin(roll_in_radians),
        0, -sin(roll_in_radians), cos(roll_in_radians));

    const glm::mat3 rotate_matrix_y(cos(pitch_in_radians), 0, -sin(pitch_in_radians),
        0, 1, 0,
        sin(pitch_in_radians), 0, cos(pitch_in_radians));

    const glm::mat3 rotate_matrix_z(cos(yaw_in_radians), sin(yaw_in_radians), 0,
        -sin(yaw_in_radians), cos(yaw_in_radians), 0,
        0, 0, 1);

    const glm::mat3 euler_rotate_matrix = rotate_matrix_z * rotate_matrix_y * rotate_matrix_x;
    direction_ = glm::normalize(euler_rotate_matrix * s_world_forward_);
    right_ = glm::normalize(euler_rotate_matrix * s_world_right_);
    up_ = glm::cross(right_, direction_);

    view_matrix_  = glm::lookAt(position_, rotation_, up_); 
}

void Camera::UpdateProjectionMatrix()
{
    projection_matrix_ = glm::perspective(glm::radians(45.0f), 2.0f, 0.1f, 10.0f);
    projection_matrix_[1][1] *= -1;
}



} // namespace engine