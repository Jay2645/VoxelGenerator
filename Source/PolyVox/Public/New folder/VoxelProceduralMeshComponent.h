// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProceduralMeshComponent.h"

// Polyvox
#include "PolyVox/PagedVolume.h"
#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"

#include "VoxelHelper.h"

#include "VoxelProceduralMeshComponent.generated.h"


USTRUCT(BlueprintType)
struct FVoxelTriangle
{
	GENERATED_BODY()
public:
	UPROPERTY()
		FVector Vertex0;
	UPROPERTY()
		FVector Vertex1;
	UPROPERTY()
		FVector Vertex2;
	UPROPERTY()
		int32 MaterialID;
};

USTRUCT(BlueprintType)
struct FVoxelMeshSection
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TArray<FVoxelTriangle> Triangles;
};

/**
 * 
 */
UCLASS()
class POLYVOX_API UVoxelProceduralMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
private:
	TSharedPtr<PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>> VoxelVolume;
	TArray<FVoxelMaterial> VoxelMaterials;
	TArray<FVoxelMeshSection> MeshSections;
	int32 CurrentTriangleIndex;

public:
	int32 TriangleBudget;

	void CreateMeshFromVolume(PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>* Volume, PolyVox::Region& ChunkRegion, TArray<FVoxelMaterial>& TerrainMaterials);
	TArray<FVoxelMeshSection> GenerateTriangles(const PolyVox::Mesh<PolyVox::MarchingCubesVertex<PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::VoxelType>>& ExtractedMesh) const;
	FProcMeshSection CreateMeshSectionData(TArray<FVoxelTriangle> Triangles, bool bShouldEnableCollision);
};
