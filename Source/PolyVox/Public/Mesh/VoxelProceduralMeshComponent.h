// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProceduralMeshComponent.h"
#include "VoxelHelper.h"
#include "Wrappers/Pager.h"
#include "Wrappers/VoxelVolume.h"
#include "VoxelProceduralMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class POLYVOX_API UVoxelProceduralMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()
private:
	FProcMeshSection CreateMeshSectionData(TArray<FVoxelTriangle> Triangles, bool bShouldEnableCollision);
public:
	// Sets the visible mesh on this component to be a marching cubes mesh.
	// The contents of the mesh are the voxels in side the VoxelVolume passed to this function.
	// The boundaries of the mesh are defined by the Region passed to ChunkRegion.
	// The materials of the mesh are the materials passed to TerrainMaterials.
	// Note that you must pass at least 1 material to Terrain Materials, or nothing will be created.
	// Additionally, if the generated mesh has more materials defined in it than the number of materials passed to TerrainMaterials, nothing will happen.
	UFUNCTION(BlueprintCallable, Category = "Voxels|Mesh")
	void CreateMarchingCubesMeshFromVolume(AVoxelVolume* Volume, URegion* ChunkRegion, TArray<FVoxelMaterial>& TerrainMaterials);

	// Creates a mesh from an array of FVoxelMeshSections.
	// Each MeshSection will be mapped to a FVoxelMaterial, based off of the materials passed into the VoxelMaterials array.
	// This function will fail if there are no mesh sections to generate or there are more MeshSections than VoxelMaterials.
	UFUNCTION(BlueprintCallable, Category = "Voxels|Mesh")
	void CreateMeshFromVoxelSections(TArray<FVoxelMeshSection> MeshSections, TArray<FVoxelMaterial> VoxelMaterials);
};
