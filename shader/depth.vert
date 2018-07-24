/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;

// Values that stay constant for the whole mesh.
uniform mat4 depth_MVP ;
uniform mat4 model_matrix ;

void main(){
	gl_Position =  depth_MVP * model_matrix * vec4(vertexPosition, 1);
}
