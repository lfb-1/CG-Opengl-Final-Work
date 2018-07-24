/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1677895 Fengbei Liu

**************************************************/
#version 330
struct Material {
	//light map properties
	sampler2D diffuse;
	sampler2D specular;
	
	float shininess;

	vec3 diffuse_property;
	vec3 ambient_property;
	vec3 specular_property;
};


uniform sampler2D shadowMap ;
in vec4 shadow_coord ;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

out vec4 fragColour;
uniform int flag;

uniform Material material;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

struct PointLight{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform PointLight pointLight;

struct SpotLight{
    vec3 position;
	vec3 direction;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float cutOff;
	float outerCutOff;
};
uniform SpotLight spotLight;

vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float calShadow();

void main(void) {
    if(flag==1){
        fragColour = vec4(1.0f);
    }else if (flag==0){
        vec3 norm = normalize(Normal);
		vec3 viewDir = normalize(viewPos - FragPos);

		vec3 result = calDirLight(dirLight,norm,viewDir);

		result += calPointLight(pointLight,norm,FragPos,viewDir);

		result += calSpotLight(spotLight,norm, FragPos,viewDir);

        //result *= calShadow() ;

		fragColour = vec4(result,1.0f);
    }
}

vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);

	//ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	//specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal,halfwayDir),0.0),material.shininess);
    vec3 specular = light.specular * spec * vec3 (texture(material.specular, TexCoords));

	return (ambient + diffuse + specular);
}

vec3 calPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    //ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    //diffuse
   
    vec3 lightDir = normalize(light.position - FragPos);  
        
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    //specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal,halfwayDir),0.0),material.shininess);
	vec3 specular = light.specular * spec * vec3 (texture(material.specular, TexCoords));

    //attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 calSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    //ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    //diffuse
   
    // CODE ALTER: Thomas' original implementation, questioned by Maximilian.
    //vec3 lightDir = viewDir;

    // Maximilian's implementation following learnOpenGL https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/5.3.light_casters_spot/5.3.light_casters.fs
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));


    //specular

    // CODE ALTER: Thomas' original implementation, questioned by Maximlian.
    //vec3 halfwayDir = normalize(lightDir + viewDir);

    // Maximilian's viewDir
    vec3 viewDirection = lightDir ;
    vec3 halfwayDir = normalize(lightDir + viewDirection);

	float spec = pow(max(dot(normal,halfwayDir),0.0),material.shininess);
	vec3 specular = light.specular * spec * vec3 (texture(material.specular, TexCoords));

    //spot
    float theta = dot(lightDir, normalize(-light.direction));   
    float epsilon = light.cutOff - light.outerCutOff;   
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   

    diffuse *= intensity;
    specular *= intensity;

    //attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}


float calShadow()
{
    float bias = 0.005 ;
    float visibility = 1.0 ;

    if ( texture(shadowMap, shadow_coord.xy).r < shadow_coord.z - bias)
    {
        visibility = 0.5 ;
    }

    return visibility ;

// Ground PCF version
/*
    float bias = 0.005 ;
    float visibility = 1.0 ;

    // Percentage-closer filtering
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, shadow_coord.xy + vec2(x, y) * texelSize).r; 
            shadow += shadow_coord.z - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
        
    visibility -= shadow ;

    return 1.0 ;
*/
}


