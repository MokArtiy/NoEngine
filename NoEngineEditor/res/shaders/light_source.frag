#version 460

out vec4 frag_color;

uniform bool stencil;

void main() {
    if(stencil)
    {
        frag_color = vec4(0.94, 0.62, 0.2, 1.0);
    }
    else
    {
        frag_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
}