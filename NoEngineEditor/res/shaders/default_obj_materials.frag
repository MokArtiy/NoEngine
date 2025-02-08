#version 460

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 ambient_factor;
    vec3 diffuse_factor;
    vec3 specular_factor;
};

in vec2 tex_coord_smile;
in vec3 frag_normal_eye;
in vec3 frag_position_eye;

layout (binding = 0) uniform sampler2D InTexture_Smile;

uniform vec3 light_position_eye;
uniform vec3 light_color;
uniform Material material;
uniform Light light; 

out vec4 frag_color;

void main() {
			  
    //ambient
	vec3 ambient = light.ambient_factor * material.ambient * light_color;
              
    //diffuse
    vec3 normal = normalize(frag_normal_eye);
    vec3 light_dir = normalize(light_position_eye - frag_position_eye);
    vec3 diffuse =  light.diffuse_factor * material.diffuse * light_color * max(dot(normal, light_dir), 0.0);

    //specular
    vec3 view_dir = normalize(-frag_position_eye);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular =  light_color * light.specular_factor * material.specular * specular_value;

    //frag_color = texture(InTexture_Smile, tex_coord_smile) * vec4(ambient + diffuse + specular, 1.f);
    frag_color = vec4(ambient + diffuse + specular, 1.f);
}