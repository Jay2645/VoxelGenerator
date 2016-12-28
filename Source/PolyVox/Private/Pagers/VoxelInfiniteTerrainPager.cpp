// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "VoxelInfiniteTerrainPager.h"

void UVoxelInfiniteTerrainPager::InitializePager_Implementation(UChunkManager* Manager)
{
	UPager::InitializePager_Implementation(Manager);

	NoiseGenerator = FastNoise();
	NoiseProfile = ChunkManager->TerrainProfiles[0];
	NoiseGenerator.SetNoiseSettings(NoiseProfile.NoiseSettings);
}

FVoxelHeightmapData UVoxelInfiniteTerrainPager::GetPointHeightmap_Implementation(URegion* VoxelRegion, int32 PointX, int32 PointY)
{
	FVoxelHeightmapData pixel;
	double currentZAmount = 0.0;
	for (int z = VoxelRegion->GetLowerZ(); z <= VoxelRegion->GetUpperZ(); z++)
	{
		currentZAmount += NoiseProfile.TerrainHeightScale + NoiseGenerator.GetNoise((float)PointX, (float)PointY, (float)z);
	}
	double normalizedHeight = currentZAmount / (double)VoxelRegion->GetDepthInVoxels();
	pixel.Biome = NoiseProfile.ProfileName;
	pixel.Elevation = normalizedHeight;
	return pixel;
}