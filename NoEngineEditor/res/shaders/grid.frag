#version 460

in vec3 ourColor;
out vec4 frag_color;

void main()
{
    frag_color = vec4(ourColor, 1.0);
}