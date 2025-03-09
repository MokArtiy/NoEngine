#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 vp;
uniform mat4 model_matrix;

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = vp * model_matrix * vec4(aPos, 1.0f);
}