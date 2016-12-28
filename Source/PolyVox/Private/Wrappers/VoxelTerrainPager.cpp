// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
//#include "VoxelChunk.h"
//#include "ChunkManager.h"
#include "Region.h"
#include "Pager.h"
#include "VoxelTerrainPager.h"

using namespace PolyVox;

VoxelTerrainPager::VoxelTerrainPager(UPager* OurPager, UChunkManager* OurChunkManager)
	: PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Pager()
{
	VoxelPager = OurPager;
	ChunkManager = OurChunkManager;
}

void VoxelTerrainPager::pageIn(const Region& VoxelRegion, PagedVolume<MaterialDensityPair44>::Chunk* Chunk)
{
	/*if (VoxelPager == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not page in region due to null pager!"));
		return;
	}
	if (Chunk == NULL)
	{
		FVector lowerCorner = region.getLowerCorner();
		FPolyVoxVector upperCorner = region.getUpperCorner();
		UE_LOG(LogTemp, Error, TEXT("Tried to page in a region from %s to %s, but the provided Chunk was null!"), *lowerCorner.ToString(), *upperCorner.ToString());
		return;
	}

	URegion* region = NewObject<URegion>(VoxelPager,TEXT("Voxel Region"));
	region->CreateRegionFromPolyVox(VoxelRegion);
	UChunk* chunk = NewObject<UChunk>(VoxelPager, TEXT("Voxel Chunk"));
	chunk->SetChunk(Chunk);
	VoxelPager->PageIn(region, chunk);*/
	
	if (Chunk == NULL || VoxelPager == NULL)
	{
		FVector lowerCorner = FPolyVoxVector(VoxelRegion.getLowerCorner());
		FVector upperCorner = FPolyVoxVector(VoxelRegion.getUpperCorner());
		UE_LOG(LogTemp, Error, TEXT("Tried to page in a region from %s to %s, but the provided Chunk was null!"), *lowerCorner.ToString(), *upperCorner.ToString());
		return;
	}

	URegion* voxelRegion = NewObject<URegion>();
	voxelRegion->CreateRegionFromPolyVox(VoxelRegion);

	UChunk* voxelChunk = NewObject<UChunk>();
	voxelChunk->SetChunk(Chunk);

	VoxelPager->PageIn(voxelRegion, voxelChunk);
}

/*FVoxelHeightmapData VoxelTerrainPager::GetPointHeightmap(const Region& VoxelRegion, int32 PointX, int32 PointY)
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
	int32 targetCell = (int32)(cellPixel.Elevation * voxelRegion->GetDepthInVoxels());
	targetCell += TerrainVoxelActor->GetChunkRegion()->GetLowerZ();
	int32 materialIndex = BiomeMaterials.FindRef(cellPixel.Biome) + 1;

	for (int z = voxelRegion->GetLowerZ(); z <= voxelRegion->GetUpperZ(); z++)
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
		FIntVector Position = FIntVector(PointX - voxelRegion->GetLowerX(), PointY - voxelRegion->GetLowerY(), z - voxelRegion->GetLowerZ());
		Chunk->setVoxel(Position.X, Position.Y, Position.Z, voxel);

		FVector chunkPoint = TerrainVoxelActor->VoxelPositionToWorldPosition(PointX, PointY, z);
		if (z == targetCell && chunkPoint.Z > WaterHeight)
		{
			TerrainVoxelActor->AddTreeLocation(cellPixel.Biome, chunkPoint);
		}
	}
}*/

void VoxelTerrainPager::pageOut(const Region& VoxelRegion, PagedVolume<MaterialDensityPair44>::Chunk* Chunk)
{
	if (VoxelPager == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not page out region due to null pager!"));
		return;
	}
	URegion* region = NewObject<URegion>(VoxelPager, TEXT("Voxel Region"));
	region->CreateRegionFromPolyVox(VoxelRegion);
	UChunk* chunk = NewObject<UChunk>(VoxelPager, TEXT("Voxel Chunk"));
	chunk->SetChunk(Chunk);
	VoxelPager->PageOut(region, chunk);
}