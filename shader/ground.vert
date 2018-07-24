/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/

#version 330 core

layout (location = 0) in vec3 vertex_position ;
layout (location = 1) in vec4 vertex_color ;
layout (location = 2) in vec2 vertex_st ;

out vec4 shadow_coord ;
out vec3 position ;
out vec2 st ;

uniform mat4 projection ;
uniform mat4 view ;
uniform mat4 model ;

uniform mat4 depth_bias_MVP ;

void main()
{
	position = vec3(model * vec4(vertex_position, 1.0)) ;
	shadow_coord = depth_bias_MVP * vec4(position, 1.0) ;
	st = vertex_st ;

	gl_Position = projection * view * vec4(position, 1.0f) ;
}
