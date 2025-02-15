#version 460

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    //sampler2D emission;
    float shininess;
};
struct Light {
    vec3 ambient_factor;
    vec3 diffuse_factor;
    vec3 specular_factor;
};

in vec2 tex_coord;
in vec3 frag_normal;
in vec3 frag_position;

//layout (binding = 0) uniform sampler2D InTexture;

uniform vec3 light_position_eye;
uniform vec3 light_color;
uniform Material material;
uniform Light light; 

out vec4 frag_color;

void main() {
			  
    //ambient
	vec3 ambient = light_color * light.ambient_factor * vec3(texture(material.diffuse, tex_coord)).rgb;
              
    //diffuse
    vec3 normal = normalize(frag_normal);
    vec3 light_dir = normalize(light_position_eye - frag_position);
    vec3 diffuse =  light_color * light.diffuse_factor * vec3(texture(material.diffuse, tex_coord)).rgb * max(dot(normal, light_dir), 0.0);

    //specular
    vec3 view_dir = normalize(-frag_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular =  light_color * light.specular_factor * vec3(texture(material.specular, tex_coord)).rgb * specular_value;

    //emission
    //vec3 emission = texture(material.emission, tex_coord).rgb;

    //frag_color = texture(InTexture, tex_coord) * vec4(ambient + diffuse + specular, 1.f);
    frag_color = vec4(ambient + diffuse + specular, 1.f);
}