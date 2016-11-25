// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PolyVox/PagedVolume.h"
#include "PolyVox/MaterialDensityPair.h"
#include "VoxelHelper.h"

class AVoxelChunk;

/**
 * 
 */
class POLYVOX_API VoxelTerrainPager : public PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Pager
{
public:
	// Constructor
	VoxelTerrainPager(FVector TerrainChunkPosition, AVoxelChunk* VoxelActor);

	// Destructor
	virtual ~VoxelTerrainPager() {};

	// PagedVolume::Pager functions
	virtual void pageIn(const PolyVox::Region& VoxelRegion, PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk* Chunk);
	virtual FVoxelHeightmapData GetPointHeightmap(const PolyVox::Region& VoxelRegion, int32 PointX, int32 PointY);
	virtual void ProcessPoint(const PolyVox::Region& VoxelRegion, PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk* Chunk, int32 PointX, int32 PointY, float WaterHeight, const TMap<FName, int32>& BiomeMaterials);
	virtual void pageOut(const PolyVox::Region& VoxelRegion, PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk* Chunk);

protected:
	FVector ChunkPosition;
	AVoxelChunk* TerrainVoxelActor;
};
