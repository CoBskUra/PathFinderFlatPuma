#version 330 core
struct Light {
    vec3 position;
    vec3 lightColor;
    float ambientStrenght;
    float diffuseStrenght;
    float specularStrenght;
};

//uniform Light;
// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the color from the Vertex Shader

uniform vec4 COLOR;
uniform vec3 CAMERA_POS;

in vec3 FragPos;
in vec3 Normal;

vec3 Phong(Light light, vec3 fragPos, vec3 viewPos, vec3 normal){
     // ambient
    //float ambientStrength = 0.1;
    vec3 ambient = light.ambientStrenght * light.lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuseStrenght * diff * light.lightColor;
    
    // specular
    //float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specularStrenght * spec * light.lightColor;  

    return (ambient + diffuse + specular);
}

void main()
{
//FragColor = vec4(1, 0, 0, 1);
    Light light;
    light.lightColor = vec3(1, 1, 1);
    light.position = vec3(0, 80, 0);
    light.ambientStrenght = 0.1;
	light.diffuseStrenght = 1;
	light.specularStrenght = 0.5;
	FragColor = vec4(Phong(light, FragPos, CAMERA_POS, Normal) *COLOR.xyz, COLOR.w );
    //FragColor = vec4(Normal * 0.5 + 1, 1);
}