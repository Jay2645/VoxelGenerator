// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PolyVox/PagedVolume.h"
#include "PolyVox/MaterialDensityPair.h"
#include "VoxelHelper.h"

class UPager;

/**
 * 
 */
class POLYVOX_API VoxelTerrainPager : public PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Pager
{
public:
	// Constructor
	VoxelTerrainPager(UPager* OurPager, UChunkManager* OurChunkManager);

	// Destructor
	virtual ~VoxelTerrainPager() {};

	// PagedVolume::Pager functions
	virtual void pageIn(const PolyVox::Region& VoxelRegion, PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk* Chunk);
	virtual void pageOut(const PolyVox::Region& VoxelRegion, PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk* Chunk);
protected:
	UPager* VoxelPager;
	UChunkManager* ChunkManager;
};
