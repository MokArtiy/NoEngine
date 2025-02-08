#version 460

in vec2 tex_coord_smile;
in vec3 frag_normal_eye;
in vec3 frag_position_eye;

layout (binding = 0) uniform sampler2D InTexture_Smile;
layout (binding = 1) uniform sampler2D InTexture_Quads;

uniform vec3 light_color;
uniform vec3 light_position_eye;
uniform float ambient_factor;
uniform float diffuse_factor;
uniform float specular_factor;
uniform float shininess;

out vec4 frag_color;

void main() {
			  
    //ambient
	vec3 ambient = ambient_factor * light_color;
              
    //diffuse
    vec3 normal = normalize(frag_normal_eye);
    vec3 light_dir = normalize(light_position_eye - frag_position_eye);
    vec3 diffuse = diffuse_factor * light_color * max(dot(normal, light_dir), 0.0);

    //specular
    vec3 view_dir = normalize(-frag_position_eye);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    vec3 specular = specular_factor * specular_value * light_color;

    //frag_color = texture(InTexture_Smile, tex_coord_smile) * texture(InTexture_Quads, tex_coord_quads);
    frag_color = texture(InTexture_Smile, tex_coord_smile) * vec4(ambient + diffuse + specular, 1.f);
}