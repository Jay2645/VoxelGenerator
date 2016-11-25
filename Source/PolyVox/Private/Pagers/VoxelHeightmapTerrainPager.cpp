// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "VoxelChunk.h"
#include "ChunkManager.h"
#include "VoxelHeightmapTerrainPager.h"

using namespace PolyVox;

FVoxelHeightmapData VoxelHeightmapTerrainPager::GetPointHeightmap(const Region& VoxelRegion, int32 PointX, int32 PointY)
{
	if (ChunkHeightmap == NULL)
	{
		return FVoxelHeightmapData();
	}

	return ChunkHeightmap->GetMapPoint(PointX, PointY);
}