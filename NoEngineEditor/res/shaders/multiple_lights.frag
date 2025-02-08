#version 460 core
out vec4 frag_color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4

in vec3 frag_position;
in vec3 frag_normal;
in vec2 tex_coord;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 frag_normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 frag_normal, vec3 frag_position, vec3 viewDir);

void main()
{    
    // properties
    vec3 norm = normalize(frag_normal);
    vec3 viewDir = normalize(- frag_position);

    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, frag_position, viewDir);     
    
    frag_color = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 frag_normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(frag_normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, frag_normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coord));
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 frag_normal, vec3 frag_position, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - frag_position);
    // diffuse shading
    float diff = max(dot(frag_normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, frag_normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - frag_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}