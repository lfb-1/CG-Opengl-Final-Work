/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/

#version 330 core

layout (location = 0) in vec3 vertex_position ;

uniform mat4 projection ;
uniform mat4 view ;
//uniform mat4 model ;

uniform vec3 offset ;


void main()
{
	vec4 pos = projection * view * vec4(vertex_position + offset, 1.0f) ;
	gl_PointSize = (1.0 - pos.z / pos.w) * 200.0f ;
    gl_Position = pos ;
}
