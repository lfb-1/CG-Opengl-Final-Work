/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1677895 Fengbei Liu

**************************************************/

#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec3 temp = aPos;
    TexCoords = vec3(temp.x,-temp.y,temp.z);
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = vec4(pos.x,pos.y,pos.w,pos.w);
}
