#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(points) in;
layout (line_strip, max_vertices = 2) out;

layout(location = 0) in vec4 cameraNormal[];

void main() {
  gl_Position = gl_in[0].gl_Position; 
  EmitVertex();

  gl_Position = gl_Position + (cameraNormal[0] * 0.2);
  EmitVertex();
  
  EndPrimitive();
}    