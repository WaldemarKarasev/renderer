#version 450

layout(binding = 0) uniform UniformBuffer
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 Position;
layout(location = 1) in vec3 Color;

layout(location = 0) out vec3 fragColor;

void main() {
    // gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(Position, 0.0, 1.0);
    fragColor = Color;
}