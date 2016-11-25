// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "VoxelChunk.h"
#include "ChunkManager.h"
#include "VoxelInfiniteTerrainPager.h"

using namespace PolyVox;

void VoxelInfiniteTerrainPager::pageIn(const Region& VoxelRegion, PagedVolume<MaterialDensityPair44>::Chunk* Chunk)
{
	UChunkManager* chunkManager = UChunkManager::GetManager(TerrainVoxelActor->GetWorld());
	NoiseProfile = chunkManager->TerrainProfiles[0];
	NoiseGenerator.SetNoiseSettings(NoiseProfile.NoiseSettings);
	VoxelTerrainPager::pageIn(VoxelRegion, Chunk);
}

FVoxelHeightmapData VoxelInfiniteTerrainPager::GetPointHeightmap(const Region& VoxelRegion, int32 PointX, int32 PointY)
{
	double currentZAmount = 0.0;
	for (int z = VoxelRegion.getLowerZ(); z <= VoxelRegion.getUpperZ(); z++)
	{
		currentZAmount += NoiseProfile.TerrainHeightScale + NoiseGenerator.GetNoise(ChunkPosition.X + (float)PointX, ChunkPosition.Y + (float)PointY, ChunkPosition.Z + (float)z);
	}
	double normalizedHeight = currentZAmount / (double)VoxelRegion.getDepthInVoxels();
	FVoxelHeightmapData pixel;
	pixel.Biome = NoiseProfile.ProfileName;
	pixel.Elevation = normalizedHeight;

	return pixel;
}