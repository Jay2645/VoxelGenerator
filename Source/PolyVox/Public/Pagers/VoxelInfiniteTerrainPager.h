// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Noise/FastNoise.h"
#include "VoxelTerrainPager.h"

/**
 * 
 */
class POLYVOX_API VoxelInfiniteTerrainPager : public VoxelTerrainPager
{
public:
	// Constructor
	VoxelInfiniteTerrainPager(FVector TerrainChunkPosition, AVoxelChunk* VoxelActor) : VoxelTerrainPager(TerrainChunkPosition, VoxelActor) {};
	~VoxelInfiniteTerrainPager() {};

public:
	FastNoise NoiseGenerator;
	FNoiseProfile NoiseProfile;

public:
	virtual void pageIn(const PolyVox::Region& VoxelRegion, PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk* Chunk) override;
	virtual FVoxelHeightmapData GetPointHeightmap(const PolyVox::Region& VoxelRegion, int32 PointX, int32 PointY) override;
};