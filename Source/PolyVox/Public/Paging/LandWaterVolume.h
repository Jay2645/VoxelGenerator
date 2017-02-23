// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Paging/PagedVolume.h"
#include "LandWaterVolume.generated.h"

/**
 * 
 */
UCLASS()
class POLYVOX_API ALandWaterVolume : public APagedVolume
{
	GENERATED_BODY()
public:
	ALandWaterVolume();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume")
	UPagedVolumeComponent* WaterPagedVolumeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxels")
	TArray<FVoxelMaterial> VolumeWaterMaterials;

	UFUNCTION(BlueprintCallable, Category = "Volume")
	UPagedVolumeComponent* GetWaterPagedVolume() const;
};
