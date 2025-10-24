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
	float4 calculatedColor = float4(
		// RGB (color)
		1.0, 1.0, 1.0,
		// Alpha (opacity)
		1.0 );
	calculatedColor.r = sqrt(pow(sin(g_elapsedSecondCount_simulationTime), 2));
	calculatedColor.g = sqrt(pow(sin(g_elapsedSecondCount_simulationTime + 2), 2));
	calculatedColor.b = sqrt(pow(sin(g_elapsedSecondCount_simulationTime + 4), 2));
	o_color = calculatedColor * i_color;
}
