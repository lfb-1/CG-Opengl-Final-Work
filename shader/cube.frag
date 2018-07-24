/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1677895 Fengbei Liu

**************************************************/

#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform float FogStart;
uniform float FogEnd;
uniform vec4 fogColor;
float LinerFog(float distance){
    float fogAlpha = (distance - FogStart) / (FogEnd - FogStart);
    fogAlpha = clamp(fogAlpha,0.0,1.0);
    return fogAlpha;
}
void main()
{    
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    vec3 temp_R = vec3(R.x,-R.y,R.z);
    float fogAlpha = LinerFog(sqrt((cameraPos.x - 6.0) * (cameraPos.x - 6.0) + (cameraPos.z - 3.0) * (cameraPos.z - 3.0))  / 1.0);
    FragColor = mix(vec4(texture(skybox, temp_R).rgb, 1.0),fogColor,fogAlpha);
}
