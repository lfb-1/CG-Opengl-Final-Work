/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1677895 Fengbei Liu

**************************************************/

#version 330

// This shader needs both a projection matrix and a view matrix
// gl_Position is the vertex location in clip coordinates

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 projection;
uniform mat4 model_matrix;
uniform mat4 view;

uniform mat4 depth_bias_MVP ;

out vec4 shadow_coord ;
out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;

void main(void) {

	shadow_coord = depth_bias_MVP * vec4(a_vertex, 1.0) ;

	// We simply pass the colour along to the next stage
	Normal = mat3(transpose(inverse(model_matrix))) * aNormal;
	TexCoords = aTexCoord;
	FragPos = vec3(model_matrix * vec4(a_vertex,1.0));
	gl_Position = projection * view * model_matrix * vec4(a_vertex, 1.0);

}
