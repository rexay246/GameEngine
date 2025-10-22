#include "cMesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Platform/Platform.h>

eae6320::cResult eae6320::Graphics::cMesh::Load(cMesh*& o_mesh, const std::string& i_path)
{
	auto result = eae6320::Results::Success;
	Platform::sDataFromFile dataFromFile;
	if (!(result = Platform::LoadBinaryFile(i_path.c_str(), dataFromFile))) {
		Logging::OutputError("Error when reading from binary file");
		o_mesh = nullptr;
		return result;
	}
	auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
	const auto finalOffset = currentOffset + dataFromFile.size;

	uint16_t vertexCount;
	memcpy(&vertexCount, reinterpret_cast<void*>(currentOffset), sizeof(vertexCount));
	currentOffset += sizeof(vertexCount);

	uint16_t indexCount;
	memcpy(&indexCount, reinterpret_cast<void*>(currentOffset), sizeof(indexCount));
	currentOffset += sizeof(indexCount);

	auto* const vertexArray = reinterpret_cast<VertexFormats::sVertex_mesh*>(currentOffset);
	currentOffset += sizeof(VertexFormats::sVertex_mesh) * vertexCount;

	auto* const indexArray = reinterpret_cast<uint16_t*>(currentOffset);
	currentOffset += sizeof(uint16_t) * indexCount;

	result = CreateMesh(o_mesh, vertexArray, vertexCount, indexArray, indexCount);

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::CreateMesh(cMesh*& o_mesh,
	eae6320::Graphics::VertexFormats::sVertex_mesh* vertexData,
	unsigned int vertexCount, uint16_t* indexData, unsigned int indexCount)
{
	auto result = Results::Success;
	cMesh* newMesh = nullptr;
	cScopeGuard scopeGuard([&o_mesh, &result, &newMesh]
		{
			if (result)
			{
				EAE6320_ASSERT(newMesh != nullptr);
				o_mesh = newMesh;
			}
			else
			{
				if (newMesh)
				{
					newMesh->DecrementReferenceCount();
					newMesh = nullptr;
				}
				o_mesh = nullptr;
			}
		});
	// Allocate a new mesh
	{
		newMesh = new cMesh();
		if (!newMesh)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh");
			Logging::OutputError("Failed to allocate memory for the mesh");
			return result;
		}
	}
	// Initialize mesh
	if (!(result = newMesh->Initialize(vertexData, vertexCount, indexData, indexCount)))
	{
		EAE6320_ASSERTF(false, "Initialization of new mesh failed");
		return result;
	}

	return result;
}

eae6320::Graphics::cMesh::~cMesh()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}