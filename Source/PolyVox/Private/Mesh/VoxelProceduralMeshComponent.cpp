// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "ChunkManager.h"
#include "VoxelProceduralMeshComponent.h"


using namespace PolyVox;

// Defines a region of voxels we want to render
// Polyvox will take that region and chop it up into chunks
// Each one of these chunks will then be paged in one at a time
// Some chunks will be fully "underground," while other chunks may be completely air
void UVoxelProceduralMeshComponent::CreateMeshFromVolume(PagedVolume<MaterialDensityPair44>* Volume, PolyVox::Region& ChunkRegion, TArray<FVoxelMaterial>& TerrainMaterials)
{
	VoxelMaterials = TerrainMaterials;
	if (VoxelMaterials.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No materials defined when sending volume to procedural mesh!"));
		return;
	}
	VoxelVolume = MakeShareable(Volume);
	CurrentTriangleIndex = 0;

	auto rawMesh = extractMarchingCubesMesh(VoxelVolume.Get(), ChunkRegion);
	MeshSections = GenerateTriangles(rawMesh);
	if (MeshSections.Num() > VoxelMaterials.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("More mesh sections are being made (%d) than there are materials defined (%d)."), MeshSections.Num(), VoxelMaterials.Num());
		return;
	}
	else if (MeshSections.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No meshes defined when sending volume to procedural mesh!"));
		return;
	}
	for (int i = 0; i < MeshSections.Num(); i++)
	{
		FProcMeshSection meshSection = CreateMeshSectionData(MeshSections[i].Triangles, VoxelMaterials[i].bShouldCreateCollision);
		SetProcMeshSection(i, meshSection);
		if (VoxelMaterials.Num() > i)
		{
			SetMaterial(i, VoxelMaterials[i].Material);
		}
		else
		{
			SetMaterial(i, VoxelMaterials[0].Material);
		}
	}
}

TArray<FVoxelMeshSection> UVoxelProceduralMeshComponent::GenerateTriangles(const PolyVox::Mesh<PolyVox::MarchingCubesVertex<PolyVox::PagedVolume<MaterialDensityPair44>::VoxelType>>& ExtractedMesh) const
{
	TArray<FVoxelMeshSection> meshSections;
	auto decodedMesh = decodeMesh(ExtractedMesh);

	// Sanity check
	if (decodedMesh.getNoOfIndices() < 3)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough vertices to create any procedural meshes!"));
		return meshSections;
	}

	for (int i = 0; i < decodedMesh.getNoOfIndices() - 2; i += 3)
	{
		// Create triangle
		FVoxelTriangle triangle;

		// Get Vertex 0
		auto index = decodedMesh.getIndex(i);
		FVector vertex = FPolyVoxVector(decodedMesh.getVertex(index).position);
		triangle.Vertex0 = vertex;

		// Get Vertex 1
		index = decodedMesh.getIndex(i + 1);
		vertex = FPolyVoxVector(decodedMesh.getVertex(index).position);
		triangle.Vertex1 = vertex;

		// Get Vertex 2
		index = decodedMesh.getIndex(i + 2);
		vertex = FPolyVoxVector(decodedMesh.getVertex(index).position);
		triangle.Vertex2 = vertex;

		// Get Material ID
		int32 materialID = decodedMesh.getVertex(index).data.getMaterial();
		triangle.MaterialID = materialID;
		if (materialID < 0)
		{
			continue;
		}
		else if (materialID >= meshSections.Num())
		{
			meshSections.SetNumZeroed(materialID + 1);
		}

		// Add to output
		meshSections[materialID].Triangles.Add(triangle);
	}
	return meshSections;
}

FProcMeshSection UVoxelProceduralMeshComponent::CreateMeshSectionData(TArray<FVoxelTriangle> Triangles, bool bShouldEnableCollision)
{
	FProcMeshSection meshSection;
	float voxelSize = UChunkManager::GetManager(GetWorld())->VoxelSize;

	meshSection.ProcVertexBuffer.Reset();
	for (int i = 0; i < Triangles.Num(); i++)
	{
		FProcMeshVertex vertex2;
		FProcMeshVertex vertex1;
		FProcMeshVertex vertex0;

		vertex0.Position = Triangles[i].Vertex0 * voxelSize;
		vertex1.Position = Triangles[i].Vertex1 * voxelSize;
		vertex2.Position = Triangles[i].Vertex2 * voxelSize;

		// Calculate the tangents of our triangle
		const FVector Edge01 = FPolyVoxVector(Triangles[i].Vertex1 - Triangles[i].Vertex0);
		const FVector Edge02 = FPolyVoxVector(Triangles[i].Vertex2 - Triangles[i].Vertex0);
		const FVector TangentX = Edge01.GetSafeNormal();
		FVector TangentZ = (Edge01 ^ Edge02).GetSafeNormal();

		// Set Vertex tangents and normals
		vertex0.Tangent = FProcMeshTangent(TangentX, false);
		vertex1.Tangent = FProcMeshTangent(TangentX, false);
		vertex2.Tangent = FProcMeshTangent(TangentX, false);
		vertex0.Normal = TangentZ;
		vertex1.Normal = TangentZ;
		vertex2.Normal = TangentZ;

		// UVs
		vertex0.UV0 = FVector2D(0.0f, 0.0f);
		vertex1.UV0 = FVector2D(0.0f, 0.0f);
		vertex2.UV0 = FVector2D(0.0f, 0.0f);

		// Vertex Colors
		vertex0.Color = FColor(255, 255, 255);
		vertex1.Color = FColor(255, 255, 255);
		vertex2.Color = FColor(255, 255, 255);

		// Update bounding box
		meshSection.SectionLocalBox += vertex0.Position;
		meshSection.SectionLocalBox += vertex1.Position;
		meshSection.SectionLocalBox += vertex2.Position;

		// Add to vertex buffer
		// We need to add the vertices of each triangle in reverse or the mesh will be upside down
		int32 index2 = meshSection.ProcVertexBuffer.Add(vertex2);
		int32 index1 = meshSection.ProcVertexBuffer.Add(vertex1);
		int32 index0 = meshSection.ProcVertexBuffer.Add(vertex0);

		// Add to index buffer
		// Again, this is done in reverse
		meshSection.ProcIndexBuffer.Add(index2);
		meshSection.ProcIndexBuffer.Add(index1);
		meshSection.ProcIndexBuffer.Add(index0);
	}

	meshSection.bEnableCollision = bShouldEnableCollision;
	return meshSection;
}