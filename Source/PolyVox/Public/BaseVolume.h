// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RegionHelper.h"
#include "BaseVolume.generated.h"

/// The BaseVolume class provides common functionality and an interface for other volume classes to implement.
/// You should not try to create an instance of this class directly. Instead you should use RawVolume or PagedVolume.
UCLASS(BlueprintType)
class POLYVOX_API ABaseVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseVolume();

	UFUNCTION(BlueprintPure, Category = "Volume|Voxels")
	virtual FVoxel GetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos);
	UFUNCTION(BlueprintPure, Category = "Volume|Voxels")
	virtual FVoxel GetVoxelByVector(const FVector& Coordinates);

	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	virtual void SetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos, const FVoxel& Voxel);
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	virtual void SetVoxelByVector(const FVector& Coordinates, const FVoxel& Voxel);

	UFUNCTION(BlueprintPure, Category = "Volume|Utility")
	virtual int32 CalculateSizeInBytes() const;

	virtual uint8 GetSideLengthPower() const;

	// Flattens a region to be exactly a specific height.
	// Any Voxels above this height are turned to air.
	// Voxels below this height are brought up to be of the specified height, using a given Voxel as a "filler".
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels|Helpers")
	void FlattenRegionToHeight(const FRegion& Region, const int32 Height, const FVoxel& Filler);

	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels|Helpers")
	void SetRegionHeightmap(const FRegion& Region, const TArray<float>& Heights, const FVoxel& Filler);

	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels|Helpers")
	void SetRegionHeightmapFromImage(const FRegion& Region, UTexture2D* Texture, const FVoxel& Filler);

	UFUNCTION(BlueprintCallable, Category = "Volume|Debug")
	void DrawVolumeAsDebug(const FRegion& DebugRegion);
};
