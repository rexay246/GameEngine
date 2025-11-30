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
		{ path = "Meshes/PlayerEntityMesh.mesh" },
		{ path = "Meshes/EnemyEntityMesh.mesh" },
		{ path = "Meshes/BreakerMesh2.mesh" },
		{ path = "Meshes/BallMesh.mesh" },
		{ path = "Meshes/AlienMesh.mesh" },
		{ path = "Meshes/AlienMesh2.mesh" },
		{ path = "Meshes/WallVerticalMesh.mesh" },
		{ path = "Meshes/WallHorizontalMesh.mesh" },
	},

	entityAI = 
	{
		{ path = "EntityAI/test.eai" },
		{ path = "EntityAI/Ball.eai" },
		{ path = "EntityAI/Alien.eai" },
		{ path = "EntityAI/Alien2.eai" },
	}
}
