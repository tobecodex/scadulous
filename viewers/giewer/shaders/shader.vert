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
layout(location = 0) out vec3 fragColor;

void main() {
  gl_Position = v.proj * v.view * m.model * vec4(vertex, 1.0);

  vec4 n = m.model * vec4(normal, 1.0);
  vec3 colour = vec3(0.75, 0.75, 0.0);
  float diffuse = dot(vec4(-1, 0, 0, 0.75), n);
  fragColor = colour * diffuse;
}