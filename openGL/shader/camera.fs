#version 330 core
out vec4 fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

uniform vec3 lightColor;
uniform vec3 cameraPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
  };
uniform Material material;

struct DirectLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
  };
uniform DirectLight directLight; 

vec3 CalculateDirectLight(DirectLight light, vec3 normal, vec3 viewDir);

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
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 ViewDir);

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
  };

uniform SpotLight spotLight;

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);



void main(){
  vec3 norm = normalize(normal);
  vec3 viewDir = normalize(cameraPos - fragPos);
  vec3 result = CalculateDirectLight(directLight, norm, viewDir);

  for (int i = 0; i < NR_POINT_LIGHTS; ++i) 
      result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
  result += CalculateSpotLight(spotLight, norm, fragPos, viewDir);
  fragColor = vec4(result, 1.0);
};


vec3 CalculateDirectLight(DirectLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
    vec3 specular = light.specular  * spec * vec3(texture(material.specular, texCoord));

    return (ambient + diffuse + specular);
  }

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    //combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse,
    texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse,
    texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
  }


vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // spotlight intensity

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;
    return (ambient + diffuse + specular);
}
