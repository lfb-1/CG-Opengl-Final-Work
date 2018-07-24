/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/

#version 330

in vec4 shadow_coord ;
in vec3 position ;
in vec2 st ;


uniform vec3 camPos ;

uniform sampler2D texMap ;
uniform sampler2D shadowMap ;

out vec4 fragColor ;

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


float evaluate_shadow() ;

void main()
{
    vec3 norm = vec3(0.0, 1.0, 0.0) ;
    vec3 viewDir = normalize(position) ;

    vec3 result = calDirLight(dirLight,norm,viewDir) ;

    result += calPointLight(pointLight,norm,position,viewDir) ;

    vec4 flashLight = vec4(calSpotLight(spotLight,norm, position,viewDir), 1.0) ;

    vec4 shadowed = vec4(result, 1.0) * evaluate_shadow() ;

    vec4 ambient = vec4(dirLight.ambient, 1.0) ;


	fragColor = ( shadowed + ambient + flashLight ) * texture(texMap, st) ;
}


vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff ;
    //specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal,halfwayDir),0.0), 96.0);
    vec3 specular = light.specular * spec ;

    return diffuse + specular ;
}

vec3 calPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    //diffuse
    vec3 lightDir = normalize(light.position - position);  
        
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff ;

    //specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal,halfwayDir),0.0),96.0);
    vec3 specular = light.specular * spec ;

    //attenuation
    float distance = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}

vec3 calSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){

    //diffuse
   
    // CODE ALTER: Thomas' original implementation, questioned by Maximilian.
    //vec3 lightDir = viewDir;

    // Maximilian's implementation following learnOpenGL https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/5.3.light_casters_spot/5.3.light_casters.fs
    vec3 lightDir = normalize(light.position - position);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff ;


    //specular

    // CODE ALTER: Thomas' original implementation, questioned by Maximlian.
    //vec3 halfwayDir = normalize(lightDir + viewDir);

    // Maximilian's viewDir
    vec3 viewDirection = lightDir ;
    vec3 halfwayDir = normalize(lightDir + viewDirection);

    float spec = pow(max(dot(normal,halfwayDir),0.0),96.0);
    vec3 specular = light.specular * spec ;

    //spot
    float theta = dot(lightDir, normalize(-light.direction));   
    float epsilon = light.cutOff - light.outerCutOff;   
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   

    diffuse *= intensity;
    specular *= intensity;

    //attenuation
    float distance = length(light.position - position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    diffuse *= attenuation;
    specular *= attenuation;

    return diffuse + specular;
}


float evaluate_shadow()
{
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
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(shadow_coord.z > 1.0) shadow = 0.0;
        
    visibility -= shadow ;


    //CODE ALTER: without PCF sampling
/*
    if ( texture(shadowMap, shadow_coord.xy).r < shadow_coord.z - bias)
    {
        visibility = 0.5 ;
    }
*/

    return visibility ;
}

