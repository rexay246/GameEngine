--[[
	This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
	shaders =
	{
		{ path = "Shaders/Fragment/animatedShader.shader", arguments = { "fragment" } },
		{ path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },
		{ path = "Shaders/Fragment/standard.shader", arguments = { "fragment" } },
		{ path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
	},

	meshes = 
	{
		{ path = "Meshes/mesh.mesh" },
		{ path = "Meshes/mesh2.mesh" },
		{ path = "Meshes/TooManyVertices.mesh" },
		{ path = "Meshes/PlayerCharacter.mesh" },
		{ path = "Meshes/SunObject.mesh" },
		{ path = "Meshes/FloorObject.mesh" },
	}
}
