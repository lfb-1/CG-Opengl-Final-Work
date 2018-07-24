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

out vec4 color ;
out vec2 st ;

void main()
{
	color = vertex_color ;
	st = vertex_st ;

	gl_Position = vec4(vertex_position, 1.0f) ;
}
