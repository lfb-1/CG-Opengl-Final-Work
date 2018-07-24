/**************************************************

The University of Adelaide
Computer Graphics Final Project 2018
a1695329 a1677895

This section's first author is a1695329 Yong Yang

**************************************************/

#version 330

uniform sampler2D sprite_texture ;

out vec4 fragColour;


void main(void)
{
	vec4 result = texture(sprite_texture, gl_PointCoord) ;

	if ( result.a < 0.1 )	discard ;

    fragColour = result ;

}
