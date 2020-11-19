#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (points) in;
layout (line_strip, max_vertices = 2) out;
  
void main() {
    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();

    gl_Position = gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();
}    