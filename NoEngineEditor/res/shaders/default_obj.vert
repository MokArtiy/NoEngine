#version 460

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coord;

uniform mat4 model_matrix;
uniform mat4 projection_view_matrix;
uniform mat3 normal_matrix;

out vec2 tex_coord;
out vec3 frag_normal;
out vec3 frag_position;

void main() {
    tex_coord = texture_coord;
    frag_normal = normal_matrix * vertex_normal;              
    frag_position = vec3(model_matrix * vec4(vertex_position, 1.0));
    gl_Position = projection_view_matrix * vec4(frag_position, 1.0);
}
