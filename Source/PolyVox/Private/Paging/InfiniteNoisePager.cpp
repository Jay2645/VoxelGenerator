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
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Water.Ocean")), oceanFloorSettings);
	FVoxelNoiseSettings iceLakebedSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Frozen.Ice")), iceLakebedSettings);
	FVoxelNoiseSettings marshLakebedSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Water.Marsh")), marshLakebedSettings);
	FVoxelNoiseSettings plainLakebedSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Water.Lake")), plainLakebedSettings);
	FVoxelNoiseSettings coastSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Coast")), coastSettings);
	FVoxelNoiseSettings snowSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Frozen.Snow")), snowSettings);
	FVoxelNoiseSettings tundraSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Frozen.Tundra")), tundraSettings);
	FVoxelNoiseSettings bareSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Bare")), bareSettings);
	FVoxelNoiseSettings scorchedSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Scorched")), scorchedSettings);
	FVoxelNoiseSettings taigaSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Frozen.Taiga")), taigaSettings);
	FVoxelNoiseSettings shrublandSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Shrubland")), shrublandSettings);
	FVoxelNoiseSettings temperateDesertSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Desert.TemperateDesert")), temperateDesertSettings);
	FVoxelNoiseSettings temperateRainForestSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Forest.TemperateRainForest")), temperateRainForestSettings);
	FVoxelNoiseSettings temperateDeciduousForestSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Forest.TemperateDeciduousForest")), temperateDeciduousForestSettings);
	FVoxelNoiseSettings grasslandSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Grassland")), grasslandSettings);
	FVoxelNoiseSettings tropicalRainForestSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Forest.TropicalRainForest")), tropicalRainForestSettings);
	FVoxelNoiseSettings tropicalSeasonalForestSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Forest.TropicalSeasonalForest")), tropicalSeasonalForestSettings);
	FVoxelNoiseSettings subtropicalDesertSettings;
	BiomeNoiseSettings.Add(FGameplayTag::RequestGameplayTag(TEXT("Voxels.Static.Desert.SubtropicalDesert")), subtropicalDesertSettings);

	bGenerateNewBiomes = true;
}

void UInfiniteNoisePager::PageIn_Implementation(const FRegion& Region, APagedChunk* Chunk)
{
	int32 regionDepth = URegionHelper::GetDepthInCells(Region);
	for (int x = Region.LowerX; x < Region.UpperX; x++)
	{
		for (int y = Region.LowerY; y < Region.UpperY; y++)
		{
			FGameplayTag chunkBiome = FGameplayTag::EmptyTag;

			FVoxel bottomVoxel = Chunk->GetVoxelByCoordinatesWorldSpace(x, y, Region.LowerZ);
			if (bottomVoxel.VoxelType.IsValid())
			{
				// The bottom voxel is solid, so iterate over the rest of the region
				FVoxel currentVoxel = bottomVoxel;
				for (int z = Region.LowerZ; z < Region.UpperZ - 1; z++)
				{
					FVoxel nextVoxel = Chunk->GetVoxelByCoordinatesWorldSpace(x, y, z + 1);
					if (!nextVoxel.VoxelType.IsValid())
					{
						// Next voxel is air, so set the biome to the top voxel
						chunkBiome = currentVoxel.VoxelType;
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
				UE_LOG(LogPolyVox, Warning, TEXT("No voxel settings for material %s!"), *chunkBiome.ToString());
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
					Chunk->SetVoxelByCoordinatesWorldSpace(x, y, z, FVoxel::MakeVoxel(chunkBiome));
				}
				else
				{
					Chunk->SetVoxelByCoordinatesWorldSpace(x, y, z, FVoxel::GetEmptyVoxel());
				}
			}
		}
	}
}
