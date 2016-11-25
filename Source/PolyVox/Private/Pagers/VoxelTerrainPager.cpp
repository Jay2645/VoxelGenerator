// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "VoxelChunk.h"
#include "ChunkManager.h"
#include "VoxelTerrainPager.h"

using namespace PolyVox;

VoxelTerrainPager::VoxelTerrainPager(FVector TerrainChunkPosition, AVoxelChunk* VoxelActor)
	: PagedVolume<MaterialDensityPair44>::Pager()
{
	ChunkPosition = TerrainChunkPosition;
	TerrainVoxelActor = VoxelActor;
}

void VoxelTerrainPager::pageIn(const Region& VoxelRegion, PagedVolume<MaterialDensityPair44>::Chunk* Chunk)
{
	if (TerrainVoxelActor == NULL || Chunk == NULL)
	{
		//FPolyVoxVector lowerCorner = region.getLowerCorner();
		//FPolyVoxVector upperCorner = region.getUpperCorner();
		//UE_LOG(LogVoxelGen, Error, TEXT("Tried to page in a region from %s to %s, but the provided Chunk was null!"), *lowerCorner.ToString(), *upperCorner.ToString());
		return;
	}

	UChunkManager* chunkManager = UChunkManager::GetManager(TerrainVoxelActor->GetWorld());

	TMap<FName, int32> biomeMaterials;
	for (int i = 0; i < chunkManager->TerrainProfiles.Num(); i++)
	{
		FName profileName = chunkManager->TerrainProfiles[i].ProfileName;
		int32 matID = chunkManager->TerrainProfiles[i].MaterialIndex;

		biomeMaterials.Add(profileName, matID);
	}

	for (int x = VoxelRegion.getLowerX(); x <= VoxelRegion.getUpperX(); x++)
	{
		for (int y = VoxelRegion.getLowerY(); y <= VoxelRegion.getUpperY(); y++)
		{
			ProcessPoint(VoxelRegion, Chunk, x, y, chunkManager->WaterHeight, biomeMaterials);
		}
	}
}

FVoxelHeightmapData VoxelTerrainPager::GetPointHeightmap(const Region& VoxelRegion, int32 PointX, int32 PointY)
{
	return FVoxelHeightmapData();
}

void VoxelTerrainPager::ProcessPoint(	const PolyVox::Region& VoxelRegion, 
										PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk* Chunk, 
										int32 PointX, 
										int32 PointY, 
										float WaterHeight, 
										const TMap<FName, int32>& BiomeMaterials)
{
	FVoxelHeightmapData cellPixel = GetPointHeightmap(VoxelRegion, PointX, PointY);
	int32 targetCell = (int32)(cellPixel.Elevation * VoxelRegion.getDepthInVoxels());
	targetCell += TerrainVoxelActor->GetChunkRegion().getLowerZ();
	int32 materialIndex = BiomeMaterials.FindRef(cellPixel.Biome) + 1;

	for (int z = VoxelRegion.getLowerZ(); z <= VoxelRegion.getUpperZ(); z++)
	{
		// Evaluate the noise 
		MaterialDensityPair44 voxel;
		bool bSolid = z <= targetCell;

		int32 voxelMaterial = 0;
		if (bSolid)
		{
			voxelMaterial = materialIndex;
			voxel.setDensity(255);
		}
		else
		{
			voxel.setDensity(0);
		}
		voxel.setMaterial(voxelMaterial);

		// Voxel position within a chunk always start from zero. So if a chunk represents region (4, 8, 12) to (11, 19, 15)
		// then the valid chunk voxels are from (0, 0, 0) to (7, 11, 3). Hence we subtract the lower corner position of the
		// region from the volume space position in order to get the chunk space position.
		FIntVector Position = FIntVector(PointX - VoxelRegion.getLowerX(), PointY - VoxelRegion.getLowerY(), z - VoxelRegion.getLowerZ());
		Chunk->setVoxel(Position.X, Position.Y, Position.Z, voxel);

		FVector chunkPoint = TerrainVoxelActor->VoxelPositionToWorldPosition(PointX, PointY, z);
		if (z == targetCell && chunkPoint.Z > WaterHeight)
		{
			TerrainVoxelActor->AddTreeLocation(cellPixel.Biome, chunkPoint);
		}
	}
}

void VoxelTerrainPager::pageOut(const Region& region, PagedVolume<MaterialDensityPair44>::Chunk* Chunk)
{

}