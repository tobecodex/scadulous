#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform View {
    mat4 view;
    mat4 proj;
} v;

layout(push_constant) uniform Model
{
    mat4 model;
} m;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 cameraNormal;

void main() {
  gl_Position = v.proj * v.view * m.model * vec4(vertex, 1.0);
  cameraNormal = v.proj * v.view * m.model * vec4(normal, 0.0);
}