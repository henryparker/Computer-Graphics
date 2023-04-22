// 1. Point Light
#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
	// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
            
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
// 2. Directional Light
// #version 330 core
// out vec4 FragColor;

// in vec3 Normal;  
// in vec3 FragPos;  
  
// uniform vec3 Directionlight; 
// uniform vec3 lightColor;
// uniform vec3 objectColor;
// uniform vec3 viewPos;

// void main()
// {
// 	// ambient
//     float ambientStrength = 0.1;
//     vec3 ambient = ambientStrength * lightColor;
  	
//     // diffuse 
//     vec3 norm = normalize(Normal);
//     vec3 lightDir = normalize(-Directionlight);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = diff * lightColor;

//     // specular
//     float specularStrength = 0.5;
//     vec3 viewDir = normalize(viewPos - FragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//     vec3 specular = specularStrength * spec * lightColor;
            
//     vec3 result = (ambient + diffuse + specular) * objectColor;
//     FragColor = vec4(result, 1.0);
// }
// 3. FlashLight
// #version 330 core
// out vec4 FragColor;

// struct Material {
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;    
//     float shininess;
// }; 

// struct Light {
//     vec3 position;  
//     vec3 direction;
//     float cutOff;
//     float outerCutOff;
  
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
	
//     float constant;
//     float linear;
//     float quadratic;
// };

// in vec3 FragPos;  
// in vec3 Normal;  
  
// uniform vec3 viewPos;
// uniform Material material;
// uniform Light light;

// void main()
// {
//     // ambient
//     vec3 ambient = light.ambient * material.ambient;
    
//     // diffuse 
//     vec3 norm = normalize(Normal);
//     vec3 lightDir = normalize(light.position - FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = light.diffuse * diff * material.diffuse;  
    
//     // specular
//     vec3 viewDir = normalize(viewPos - FragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);  
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = light.specular * spec * material.specular;

//     // spotlight (smooth edges)
//     float theta = dot(lightDir, normalize(-light.direction));
//     float epsilon = (light.cutOff - light.outerCutOff);
//     float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//     diffuse *= intensity;
//     specular *= intensity;
    
//     // attenuation
//     float distance    = length(light.position - FragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
//     ambient  *= attenuation; 
//     diffuse  *= attenuation;
//     specular *= attenuation;  

        
//     vec3 result = ambient + diffuse + specular;
//     FragColor = vec4(result, 1.0);
// } 