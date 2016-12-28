// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "VoxelChunk.h"
#include "DrawDebugHelpers.h"
#include "VoxelVolume.h"

using namespace PolyVox;

AVoxelVolume::AVoxelVolume()
{
	PagerType = UPager::StaticClass();
}

void AVoxelVolume::BeginPlay()
{
	UPager* pager = NewObject<UPager>(this, PagerType);
	pager->InitializePager(UChunkManager::GetManager(GetWorld()));
	Volume = MakeShareable(new PagedVolume<MaterialDensityPair44>(pager->GetPager()));

	URegion* region = NewObject<URegion>();
	region->CreateRegion(GetActorLocation(), GetActorLocation() + FVector(512, 512, 512));
	CreateChunkRegion(region);
}

TArray<FVoxelMeshSection> AVoxelVolume::GenerateTriangles(const PolyVox::Mesh<PolyVox::Vertex<PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::VoxelType>>& DecodedMesh) const
{
	TArray<FVoxelMeshSection> meshSections;

	// Sanity check
	if (DecodedMesh.getNoOfIndices() < 3)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough vertices to create any procedural meshes!"));
		return meshSections;
	}

	for (int i = 0; i < DecodedMesh.getNoOfIndices() - 2; i += 3)
	{
		// Create triangle
		FVoxelTriangle triangle;

		// Get Vertex 0
		auto index = DecodedMesh.getIndex(i);
		FVector vertex = FPolyVoxVector(DecodedMesh.getVertex(index).position);
		triangle.Vertex0 = vertex;

		// Get Vertex 1
		index = DecodedMesh.getIndex(i + 1);
		vertex = FPolyVoxVector(DecodedMesh.getVertex(index).position);
		triangle.Vertex1 = vertex;

		// Get Vertex 2
		index = DecodedMesh.getIndex(i + 2);
		vertex = FPolyVoxVector(DecodedMesh.getVertex(index).position);
		triangle.Vertex2 = vertex;

		// Get Material ID
		int32 materialID = DecodedMesh.getVertex(index).data.getMaterial();
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

void AVoxelVolume::CreateChunkRegion(URegion* ChunkRegion)
{
	AVoxelChunk::GenerateTerrain(this, ChunkRegion);	
}

TArray<FVoxelMeshSection> AVoxelVolume::CreateMarchingCubesMeshSections(URegion* ChunkRegion) const
{
	TArray<FVoxelMeshSection> triangles;
	if (ChunkRegion == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not create mesh because region was null!"))
		return triangles;
	}
	PagedVolume<MaterialDensityPair44>* regionVolume = Volume.Get();
	if (regionVolume == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not create mesh because volume was null!"));
		return triangles;
	}
	Region chunkRegion = ChunkRegion->GetRegion();

	regionVolume->prefetch(chunkRegion);

	auto rawMesh = extractMarchingCubesMesh(regionVolume, chunkRegion);
	//auto rawMesh = extractCubicMesh(regionVolume, chunkRegion);
	auto decodedMesh = decodeMesh(rawMesh);
	triangles = GenerateTriangles(decodedMesh);

	return triangles;
}

void AVoxelVolume::SetVoxel(int32 XCoord, int32 YCoord, int32 ZCoord, int32 Material, int32 Density)
{
	PolyVox::MaterialDensityPair44 voxel;
	voxel.setMaterial(Material);
	voxel.setDensity(Density);
	Volume->setVoxel(XCoord, YCoord, ZCoord, voxel);
}

int32 AVoxelVolume::GetVoxelMaterial(int32 XCoord, int32 YCoord, int32 ZCoord) const
{
	return Volume->getVoxel(XCoord, YCoord, ZCoord).getMaterial();
}

int32 AVoxelVolume::GetVoxelDensity(int32 XCoord, int32 YCoord, int32 ZCoord) const
{
	return Volume->getVoxel(XCoord, YCoord, ZCoord).getDensity();
}

void AVoxelVolume::DrawDebugChunk(URegion* RegionToDraw) const
{
	TArray<FVoxelMeshSection> regions = CreateMarchingCubesMeshSections(RegionToDraw);
	for (int i = 0; i < regions.Num(); i++)
	{
		TArray<FVoxelTriangle> triangles = regions[i].Triangles;
		if (triangles.Num() == 0)
		{
			continue;
		}
		for (int j = 0; j < triangles.Num(); j++)
		{
			DrawDebugLine(GetWorld(), triangles[j].Vertex0, triangles[j].Vertex1, FColor::Blue, true);
			DrawDebugLine(GetWorld(), triangles[j].Vertex1, triangles[j].Vertex2, FColor::Blue, true);
			DrawDebugLine(GetWorld(), triangles[j].Vertex2, triangles[j].Vertex0, FColor::Blue, true);
		}
		break;
	}
}