// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Pager.h"
#include "Region.h"
#include "VoxelHelper.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/MaterialDensityPair.h"
#include "VoxelVolume.generated.h"

/// This class provide a volume implementation which avoids storing all the data in memory at all times. Instead it breaks the volume
/// down into a set of chunks and moves these into and out of memory on demand. Internally, this class uses PolyVox's PagedVolume class.
/// 
/// The PagedVolume makes use of a Pager which defines the source and/or destination for data paged into and out of memory.
/// A consequence of this paging approach is that the PagedVolume does not need to have a predefined size. After the volume has been 
/// created you can begin accessing voxels anywhere in space and the required data will be created automatically.
UCLASS(BlueprintType)
class POLYVOX_API AVoxelVolume : public AActor
{
	GENERATED_BODY()

private:
	TSharedPtr<PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>> Volume;
	TArray<FVoxelMeshSection> GenerateTriangles(const PolyVox::Mesh<PolyVox::Vertex<PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::VoxelType>>& DecodedMesh) const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pager")
		TSubclassOf<UPager> PagerType;

	AVoxelVolume();
	virtual void BeginPlay() override;

	void CreateChunkRegion(URegion* ChunkRegion);

public:
	// Mesh	

	// Creates an array of FVoxelMeshSections which represent a mesh generated using the "marching cubes" algorithm.
	// The contents of the mesh will be the voxels present within the attached region.
	// This array can be passed to a VoxelProcedrualMeshComponent to generate a mesh.
	UFUNCTION(BlueprintCallable, Category = "Voxels|Mesh")
	TArray<FVoxelMeshSection> CreateMarchingCubesMeshSections(URegion* ChunkRegion) const;

	// Sets the voxel at the given coordinates.
	// The Material passed to this function should match up to the 
	UFUNCTION(BlueprintCallable, Category = "Voxels")
	void SetVoxel(int32 XCoord, int32 YCoord, int32 ZCoord, int32 Material, int32 Density);
	UFUNCTION(BlueprintPure, Category = "Voxels")
	int32 GetVoxelMaterial(int32 XCoord, int32 YCoord, int32 ZCoord) const;
	UFUNCTION(BlueprintPure, Category = "Voxels")
	int32 GetVoxelDensity(int32 XCoord, int32 YCoord, int32 ZCoord) const;
	
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DrawDebugChunk(URegion* RegionToDraw) const;
};
