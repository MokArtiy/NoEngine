#version 460

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coord;

uniform mat4 model_view_matrix;
uniform mat4 mvp_matrix;
uniform mat3 normal_matrix;

out vec2 tex_coord;
out vec3 frag_normal_eye;
out vec3 frag_position_eye;

void main() {
    tex_coord = texture_coord;
    frag_normal_eye = normal_matrix * vertex_normal;              
    frag_position_eye = vec3(model_view_matrix * vec4(vertex_position, 1.0));
    gl_Position = mvp_matrix * vec4(vertex_position, 1.0);
}