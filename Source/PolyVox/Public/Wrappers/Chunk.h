// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/MaterialDensityPair.h"
#include "Chunk.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POLYVOX_API UChunk : public UObject
{
	GENERATED_BODY()
private:
	TSharedPtr<PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk> PagedChunk;
public:
	void SetChunk(PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk* PolyVoxChunk);

	UFUNCTION(BlueprintCallable, Category = "Voxels")
	void SetVoxel(int32 XCoord, int32 YCoord, int32 ZCoord, int32 Material, int32 Density);
	void SetVoxelFromPolyVox(int32 XCoord, int32 YCoord, int32 ZCoord, PolyVox::MaterialDensityPair44 Voxel);

	UFUNCTION(BlueprintPure, Category = "Voxels")
	int32 GetVoxelMaterial(int32 XCoord, int32 YCoord, int32 ZCoord) const;
	UFUNCTION(BlueprintPure, Category = "Voxels")
	int32 GetVoxelDensity(int32 XCoord, int32 YCoord, int32 ZCoord) const;
};
