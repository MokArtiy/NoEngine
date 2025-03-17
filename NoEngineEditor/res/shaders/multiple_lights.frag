#version 460 core
out vec4 frag_color;

layout(binding = 1) uniform sampler2D diffuse_texture;
layout(binding = 2) uniform sampler2D specular_texture;

struct Material {
    float shininess;
    vec3 ambient_color; 
    vec3 diffuse_color; 
    vec3 specular_color;
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

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};



in vec3 frag_position;
in vec3 frag_normal;
in vec2 tex_coord;

#define NR_POINT_LIGHTS 100

uniform int POINT_COUNT;
uniform vec3 view_position;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform bool check_dirLight;
uniform bool check_SpotLight;
uniform bool use_texture;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 frag_normal, vec3 view_dir);
vec3 CalcPointLight(PointLight light, vec3 frag_normal, vec3 frag_position, vec3 view_dir);
vec3 CalcSpotLight(SpotLight light, vec3 frag_normal, vec3 frag_position, vec3 view_dir);

void main()
{    
    // properties
    vec3 normal = normalize(frag_normal);
    vec3 view_dir = normalize(view_position - frag_position);

    vec3 result;
    // phase 1: directional lighting
    if(check_dirLight)
    {
        result += CalcDirLight(dirLight, normal, view_dir);
    }
    // phase 2: point lights
    for(int i = 0; i < POINT_COUNT; i++)
    {
        result += CalcPointLight(pointLights[i], normal, frag_position, view_dir);
    }
 
    // phase 3: spot light
    if(check_SpotLight)
    {
        result += CalcSpotLight(spotLight, normal, frag_position, view_dir); 
    }
    
    frag_color = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 frag_normal, vec3 view_dir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(frag_normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, frag_normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient,  diffuse, specular;
    if(use_texture)
    {
        ambient = light.ambient * vec3(texture(diffuse_texture, tex_coord));
        diffuse = light.diffuse * diff * vec3(texture(diffuse_texture, tex_coord));
        specular = light.specular * spec * vec3(texture(specular_texture, tex_coord));
    }
    else{
        ambient = light.ambient * material.ambient_color;
        diffuse = light.diffuse * diff * material.diffuse_color;
        specular = light.specular * spec * material.specular_color;
    }
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 frag_normal, vec3 frag_position, vec3 view_dir)
{
    vec3 lightDir = normalize(light.position - frag_position);
    // diffuse shading
    float diff = max(dot(frag_normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, frag_normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - frag_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient,  diffuse, specular;
    if(use_texture)
    {
        ambient = light.ambient * vec3(texture(diffuse_texture, tex_coord));
        diffuse = light.diffuse * diff * vec3(texture(diffuse_texture, tex_coord));
        specular = light.specular * spec * vec3(texture(specular_texture, tex_coord));
    }
    else{
        ambient = light.ambient * material.ambient_color;
        diffuse = light.diffuse * diff * material.diffuse_color;
        specular = light.specular * spec * material.specular_color;
    }

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 frag_normal, vec3 frag_position, vec3 view_dir)
{
    vec3 lightDir = normalize(light.position - frag_position);
    // diffuse shading
    float diff = max(dot(frag_normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, frag_normal);
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - frag_position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient,  diffuse, specular;
    if(use_texture)
    {
        ambient = light.ambient * vec3(texture(diffuse_texture, tex_coord));
        diffuse = light.diffuse * diff * vec3(texture(diffuse_texture, tex_coord));
        specular = light.specular * spec * vec3(texture(specular_texture, tex_coord));
    }
    else{
        ambient = light.ambient * material.ambient_color;
        diffuse = light.diffuse * diff * material.diffuse_color;
        specular = light.specular * spec * material.specular_color;
    }
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}