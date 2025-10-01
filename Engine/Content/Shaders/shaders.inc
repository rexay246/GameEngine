/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )
	#define mat4 float4x4
	#define vec2 float2
	#define vec4 float4
	#define TransformVectorWithMatrix(g_transform_cameraToProjected, g_transform_worldToCamera, vertexPosition_world) o_vertexPosition_projected = mul(g_transform_cameraToProjected, mul(g_transform_worldToCamera, vertexPosition_world))
	#define MainDefineVertexShader main(in const float3 i_vertexPosition_local : POSITION,out float4 o_vertexPosition_projected : SV_POSITION)
	#define OutputPos
	#define CalculateVertexPositionWorld(g_transform_localToWorld, vertexPosition_local) vertexPosition_world = mul(g_transform_localToWorld, vertexPosition_local)
	#define MainDefineFragmentShader main(in const float4 i_fragmentPosition : SV_POSITION, out float4 o_color : SV_TARGET)
	#define OutputColor

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name
	#define float4x4 mat4
	#define float2 vec2
	#define float4 vec4
	#define TransformVectorWithMatrix(g_transform_cameraToProjected, g_transform_worldToCamera, vertexPosition_world) gl_Position = g_transform_cameraToProjected * (g_transform_worldToCamera * vertexPosition_world)
	#define MainDefineVertexShader main()
	#define OutputPos layout( location = 0 ) in vec3 i_vertexPosition_local;
	#define CalculateVertexPositionWorld(g_transform_localToWorld, vertexPosition_local) vertexPosition_world = g_transform_localToWorld * vertexPosition_local
	#define MainDefineFragmentShader main()
	#define OutputColor out vec4 o_color;


#endif

DeclareConstantBuffer(g_constantBuffer_frame, 0)
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For vec4 alignment
	float2 g_padding;
};

DeclareConstantBuffer(g_constantBuffer_drawCall, 2)
{
	float4x4 g_transform_localToWorld;
};

