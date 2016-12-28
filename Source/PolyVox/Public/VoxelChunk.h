// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Mesh/VoxelProceduralMeshComponent.h"
#include "Wrappers/Pager.h"
#include "Wrappers/Region.h"
#include "VoxelChunk.generated.h"

UCLASS(BlueprintType)
class POLYVOX_API AVoxelChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelChunk();
public:
	// Where our chunk is located in the map
	UPROPERTY(Category = "Voxel Terrain", BlueprintReadWrite, VisibleAnywhere)
	FVector ChunkPosition;
	// The X ID for this chunk.
	UPROPERTY(Category = "Voxel Actor", BlueprintReadWrite, VisibleAnywhere)
	int32 XID;
	// The Y ID for this chunk.
	UPROPERTY(Category = "Voxel Actor", BlueprintReadWrite, VisibleAnywhere)
	int32 YID;

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void SetChunkID(const int32& idX, const int32& idY);
	UFUNCTION(BlueprintPure, Category = "Voxel")
	FVector VoxelPositionToWorldPosition(int32 X, int32 Y, int32 Z) const;
public:
	// Mesh
	UPROPERTY(Category = "Voxel Terrain", BlueprintReadWrite, EditAnywhere)
	UVoxelProceduralMeshComponent* TerrainMesh;

	static AVoxelChunk* GenerateTerrain(AVoxelVolume* VoxelVolume, URegion* OurRegion);

public:
	// PolyVox

	// The region of this chunk.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PolyVox")
	URegion* ChunkRegion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PolyVox")
	TSubclassOf<UPager> PagerClass;
	UFUNCTION(BlueprintPure, Category = "Region")
	URegion* GetChunkRegion() const;

private:
	// Trees

	// All possible tree locations.
	TMap<FName, TArray<FVector>> PossibleTreeLocations;

public:
	UFUNCTION(BlueprintCallable, Category = "Voxels|Trees")
	void AddTreeLocation(FName Biome, FVector ChunkPoint);
};