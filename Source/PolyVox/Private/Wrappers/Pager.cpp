// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "PolyVox/DefaultMarchingCubesController.h"
#include "Pager.h"

using namespace PolyVox;

void UPager::InitializePager_Implementation(UChunkManager* Manager)
{
	ChunkManager = Manager;
	VoxelPager = MakeShareable(new VoxelTerrainPager(this, ChunkManager));
}

VoxelTerrainPager* UPager::GetPager() const
{
	return VoxelPager.Get();
}

void UPager::PageIn_Implementation(URegion* VoxelRegion, UChunk* Chunk)
{
	if (Chunk == NULL)
	{
		FVector lowerCorner = VoxelRegion->GetLowerCorner();
		FVector upperCorner = VoxelRegion->GetUpperCorner();
		UE_LOG(LogTemp, Error, TEXT("Tried to page in a region from %s to %s, but the provided Chunk was null!"), *lowerCorner.ToString(), *upperCorner.ToString());
		return;
	}

	for (int x = VoxelRegion->GetLowerX(); x <= VoxelRegion->GetUpperX(); x++)
	{
		for (int y = VoxelRegion->GetLowerY(); y <= VoxelRegion->GetUpperY(); y++)
		{
			ProcessPoint(VoxelRegion,Chunk, x, y);
		}
	}

	/*for (int x = VoxelRegion->GetLowerX(); x <= VoxelRegion->GetUpperX(); x++)
	{
		for (int y = VoxelRegion->GetLowerY(); y <= VoxelRegion->GetUpperY(); y++)
		{
			ProcessPoint(VoxelRegion, Chunk, x, y);
		}
	}*/
}

void UPager::PageOut_Implementation(URegion* VoxelRegion, UChunk* Chunk)
{

}

FVoxelHeightmapData UPager::GetPointHeightmap_Implementation(URegion* VoxelRegion, int32 PointX, int32 PointY)
{
	FastNoise NoiseGenerator = FastNoise();
	FNoiseProfile NoiseProfile = ChunkManager->TerrainProfiles[0];
	NoiseGenerator.SetNoiseSettings(NoiseProfile.NoiseSettings);

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
	//return FVoxelHeightmapData();
}

void UPager::ProcessPoint_Implementation(URegion* VoxelRegion, UChunk* Chunk, int32 PointX, int32 PointY)
{

	TMap<FName, int32> biomeMaterials;
	for (int i = 0; i < ChunkManager->TerrainProfiles.Num(); i++)
	{
		FName profileName = ChunkManager->TerrainProfiles[i].ProfileName;
		int32 matID = ChunkManager->TerrainProfiles[i].MaterialIndex;

		biomeMaterials.Add(profileName, matID);
	}


	FVoxelHeightmapData pixel = GetPointHeightmap(VoxelRegion, PointX, PointY);

	int32 targetCell = (int32)(pixel.Elevation * VoxelRegion->GetDepthInVoxels());
	//targetCell += TerrainVoxelActor->GetChunkRegion()->GetLowerZ();
	int32 materialIndex = biomeMaterials.FindRef(pixel.Biome) + 1;

	for (int z = VoxelRegion->GetLowerZ(); z < VoxelRegion->GetUpperZ(); z++)
	{
		// Evaluate the noise 
		MaterialDensityPair44 voxel;
		bool bSolid = z <= targetCell;

		int32 voxelMaterial = 0;
		//if (bSolid)
		{
			voxelMaterial = materialIndex;
			voxel.setDensity(255);
		}
		/*else
		{
			voxel.setDensity(0);
		}*/
		voxel.setMaterial(voxelMaterial);

		// Voxel position within a chunk always start from zero. So if a chunk represents region (4, 8, 12) to (11, 19, 15)
		// then the valid chunk voxels are from (0, 0, 0) to (7, 11, 3). Hence we subtract the lower corner position of the
		// region from the volume space position in order to get the chunk space position.
		FIntVector Position = FIntVector(PointX - VoxelRegion->GetLowerX(), PointY - VoxelRegion->GetLowerY(), z - VoxelRegion->GetLowerZ());
		Chunk->SetVoxelFromPolyVox(Position.X, Position.Y, Position.Z, voxel);

		/*FVector chunkPoint = TerrainVoxelActor->VoxelPositionToWorldPosition(x, y, z);
		if (z == targetCell && chunkPoint.Z > chunkManager->WaterHeight)
		{
		TerrainVoxelActor->AddTreeLocation(cellPixel.Biome, chunkPoint);
		}*/
	}
}