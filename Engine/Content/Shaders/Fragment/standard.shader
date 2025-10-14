/*
	This is the standard fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>

// Entry Point
//============

InputColor

OutputColor

void MainDefineFragmentShader
{
	// Output solid white
	o_color = (vec4) i_color;
}