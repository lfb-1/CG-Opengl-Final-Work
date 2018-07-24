/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/

#version 330

in vec4 color ;
in vec2 st ;

uniform sampler2D texMap ;

out vec4 fragColor ;

void main()
{
	//CODE ALTER Depth debugger. Modify along with frame rendered for secondary camera to show depth.
	//float depthValue = texture(texMap, st).r;
    //fragColor = vec4(vec3(depthValue), 1.0);
    

    // Use this line for none-debug mode, or normal mode.
	fragColor = texture(texMap, st) ;
}
