// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Noise/FastNoise.h"
#include "Wrappers/Pager.h"
#include "VoxelInfiniteTerrainPager.generated.h"

/**
 * 
 */
 UCLASS(Blueprintable)
class POLYVOX_API UVoxelInfiniteTerrainPager : public UPager
{
GENERATED_BODY()

public:
	FastNoise NoiseGenerator;
	FNoiseProfile NoiseProfile;

protected:
	virtual void InitializePager_Implementation(UChunkManager* Manager) override;
	virtual FVoxelHeightmapData GetPointHeightmap_Implementation(URegion* VoxelRegion, int32 PointX, int32 PointY) override;
};