#pragma once

// glm
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
        Camera()
        {
            transform_.translation.z = -2.5f;
        }

        void SetOrthographicProjection(
            float left, float right, float top, float bottom, float near, float far);
        void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

        void SetViewDirection(
            glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void SetViewTarget(
            glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);
        

        const glm::mat4& GetProjectionMatrix() const { return projectionMatrix_; }
        const glm::mat4& GetViewMatrix() const { return viewMatrix_; }
        const glm::mat4& GetInverseViewMatrix() const { return inverseViewMatrix_; }
        const glm::vec3  GetPosition() const { return glm::vec3(inverseViewMatrix_[3]); }


        private:
        glm::mat4 projectionMatrix_{1.f};
        glm::mat4 viewMatrix_{1.f};
        glm::mat4 inverseViewMatrix_{1.f};

        // This part is for moving camera with buttons in Sandbox Layer
        public: 
        void MoveInPlaneXZ(glm::vec3& rotate, glm::vec3& moveDir);
        glm::vec3 GetCameraTranslation() { return transform_.translation; }
        glm::vec3 GetCameraRotation()    { return transform_.rotation; }

        private:
        TransformComponent transform_;
        float move_speed_ = 1.5f;
        float look_speed_ = 1.5f;
};

} // namespace engine
