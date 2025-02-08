#version 460

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec2 tex_coord;
in vec3 frag_normal;
in vec3 frag_position;

uniform vec3 light_position;
//uniform vec3 light_color;
uniform Material material;
uniform Light light; 

out vec4 frag_color;

void main() {
			  
    //ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coord)).rgb;
              
    //diffuse
    vec3 normal = normalize(frag_normal);
    vec3 light_dir = normalize(light_position - frag_position);
    vec3 diffuse =  light.diffuse * vec3(texture(material.diffuse, tex_coord)).rgb * max(dot(normal, light_dir), 0.0);

    //specular
    vec3 view_dir = normalize(-frag_position);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular =  light.specular * vec3(texture(material.specular, tex_coord)).rgb * specular_value;

    // attenuation
    float distance = length(light_position - frag_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    frag_color = vec4(ambient + diffuse + specular, 1.f);
}