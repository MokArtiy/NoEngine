#version 460

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coord;

uniform mat4 projection_view;
uniform mat4 model;

void main()
{
    gl_Position = projection_view * model * vec4(vertex_position, 1.0);
}