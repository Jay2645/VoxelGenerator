// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Paging/Pager.h"
#include "Noise/PolyVoxNoise.h"
#include "InfiniteNoisePager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class POLYVOX_API UInfiniteNoisePager : public UPager
{
	GENERATED_BODY()
public:
	UInfiniteNoisePager();

	/**
	* Settings for the noise generator on a per-biome basis.
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biomes")
	TMap<uint8, FVoxelNoiseSettings> BiomeNoiseSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biomes")
	bool bGenerateNewBiomes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biomes")
	FVoxelNoiseSettings BiomeSelectorNoiseSettings;

protected:
	virtual void PageIn_Implementation(const FRegion& Region, APagedChunk* Chunk) override;
};
