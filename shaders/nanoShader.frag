
#version 330 core

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {

	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	
	vec3 color;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

uniform PointLight light;
uniform Material material;
uniform vec3 viewPos; 


in vec3 Normal;  
in vec3 FragPos;
in vec2 TexCoords;  
out vec4 color;




vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
	// ambient
    vec3 ambient = light.ambient * material.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.diffuse * vec3(texture(material.texture_diffuse1, TexCoords));
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.specular * material.specular * vec3(texture(material.texture_specular1, TexCoords));  
        
    vec3 result = (ambient + diffuse + specular) * material.color;
    
    return result;
}




void main()
{

	vec3 totalLight;
	
	//totalLight = calculateDirectionalLight(light, Normal, FragPos
	totalLight += calculatePointLight(light, Normal, FragPos, viewPos);
    
    color = vec4(totalLight, 1.0f);
} 

