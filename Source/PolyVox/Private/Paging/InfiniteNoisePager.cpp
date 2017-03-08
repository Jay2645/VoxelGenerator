// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "InfiniteNoisePager.h"

UInfiniteNoisePager::UInfiniteNoisePager()
{
	/**
	* Total list of biomes:
	*		0 - OCEAN
	*		1 - ICE
	*		2 - MARSH
	*		3 - LAKE
	*		4 - COAST
	*		5 - SNOW
	*		6 - TUNDRA
	*		7 - BARE
	*		8 - SCORCHED
	*		9 - TAIGA
	*		10 - SHRUBLAND
	*		11 - TEMPERATE DESERT
	*		12 - TEMPERATE RAIN FOREST
	*		13 - TEMPERATE DECIDUOUS FOREST
	*		14 - GRASSLAND
	*		15 - TROPICAL RAIN FOREST
	*		16 - TROPICAL SEASONAL FOREST
	*		17 - SUBTROPICAL DESERT
	*/
	FVoxelNoiseSettings oceanFloorSettings;
	BiomeNoiseSettings.Add(0, oceanFloorSettings);
	FVoxelNoiseSettings iceLakebedSettings;
	BiomeNoiseSettings.Add(1, iceLakebedSettings);
	FVoxelNoiseSettings marshLakebedSettings;
	BiomeNoiseSettings.Add(2, marshLakebedSettings);
	FVoxelNoiseSettings plainLakebedSettings;
	BiomeNoiseSettings.Add(3, plainLakebedSettings);
	FVoxelNoiseSettings coastSettings;
	BiomeNoiseSettings.Add(4, coastSettings);
	FVoxelNoiseSettings snowSettings;
	BiomeNoiseSettings.Add(5, snowSettings);
	FVoxelNoiseSettings tundraSettings;
	BiomeNoiseSettings.Add(6, tundraSettings);
	FVoxelNoiseSettings bareSettings;
	BiomeNoiseSettings.Add(7, bareSettings);
	FVoxelNoiseSettings scorchedSettings;
	BiomeNoiseSettings.Add(8, scorchedSettings);
	FVoxelNoiseSettings taigaSettings;
	BiomeNoiseSettings.Add(9, taigaSettings);
	FVoxelNoiseSettings shrublandSettings;
	BiomeNoiseSettings.Add(10, shrublandSettings);
	FVoxelNoiseSettings temperateDesertSettings;
	BiomeNoiseSettings.Add(11, temperateDesertSettings);
	FVoxelNoiseSettings temperateRainForestSettings;
	BiomeNoiseSettings.Add(12, temperateRainForestSettings);
	FVoxelNoiseSettings temperateDeciduousForestSettings;
	BiomeNoiseSettings.Add(13, temperateDeciduousForestSettings);
	FVoxelNoiseSettings grasslandSettings;
	BiomeNoiseSettings.Add(14, grasslandSettings);
	FVoxelNoiseSettings tropicalRainForestSettings;
	BiomeNoiseSettings.Add(15, tropicalRainForestSettings);
	FVoxelNoiseSettings tropicalSeasonalForestSettings;
	BiomeNoiseSettings.Add(16, tropicalSeasonalForestSettings);
	FVoxelNoiseSettings subtropicalDesertSettings;
	BiomeNoiseSettings.Add(17, subtropicalDesertSettings);

	bGenerateNewBiomes = true;
}

void UInfiniteNoisePager::PageIn_Implementation(const FRegion& Region, APagedChunk* Chunk)
{
	int32 regionDepth = URegionHelper::GetDepthInCells(Region);
	for (int x = Region.LowerX; x < Region.UpperX; x++)
	{
		for (int y = Region.LowerY; y < Region.UpperY; y++)
		{
			uint8 chunkBiome = 0;

			FVoxel bottomVoxel = Chunk->GetVoxelByCoordinatesWorldSpace(x, y, Region.LowerZ);
			if (bottomVoxel.bIsSolid)
			{
				// The bottom voxel is solid, so iterate over the rest of the region
				FVoxel currentVoxel = bottomVoxel;
				for (int z = Region.LowerZ; z < Region.UpperZ - 1; z++)
				{
					FVoxel nextVoxel = Chunk->GetVoxelByCoordinatesWorldSpace(x, y, z + 1);
					if (!nextVoxel.bIsSolid)
					{
						// Next voxel is air, so set the biome to the top voxel
						chunkBiome = currentVoxel.Material;
						break;
					}
					currentVoxel = nextVoxel;
				}
			}
			else if(bGenerateNewBiomes)
			{
				// No bottom voxel, so we need to generate a biome
				unimplemented();
				// TODO: Generate new biome based off of noise generator
			}
			

			FVoxelNoiseSettings* currentSettings = BiomeNoiseSettings.Find(chunkBiome);
			if (currentSettings == NULL)
			{
				UE_LOG(LogPolyVox, Warning, TEXT("No voxel settings for material %d!"), chunkBiome);
				break;
			}
			FVoxelNoiseSettings biomeSettings = *currentSettings;
			biomeSettings.Seed = Chunk->RandomSeed;

			PolyVoxNoise noiseGen;
			noiseGen.SetNoiseSettings(biomeSettings);

			float elevation = noiseGen.GetNoise((float)x, (float)y);

			int32 targetHeight = Region.LowerZ + FMath::RoundToInt(regionDepth * elevation);
			for (int z = Region.LowerZ; z < Region.UpperZ; z++)
			{
				if (z <= targetHeight)
				{
					Chunk->SetVoxelByCoordinatesWorldSpace(x, y, z, FVoxel::MakeVoxel(chunkBiome, true));
				}
				else
				{
					Chunk->SetVoxelByCoordinatesWorldSpace(x, y, z, FVoxel::GetEmptyVoxel());
				}
			}
		}
	}
}
