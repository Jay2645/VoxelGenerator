/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 David Williams and Matthew Williams
Modified for use in Unreal Engine 4 by Jay Stevens

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

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
	virtual UVoxel* GetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos);
	UFUNCTION(BlueprintPure, Category = "Volume|Voxels")
	virtual UVoxel* GetVoxelByVector(const FVector& Coordinates);

	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	virtual void SetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos, UVoxel* Voxel);
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	virtual void SetVoxelByVector(const FVector& Coordinates, UVoxel* Voxel);


	UFUNCTION(BlueprintPure, Category = "Volume|Utility")
	virtual bool RegionIsEmpty(const FRegion& Region);

	UFUNCTION(BlueprintPure, Category = "Volume|Utility")
	virtual int32 CalculateSizeInBytes() const;

	virtual uint8 GetSideLengthPower() const;

	// Flattens a region to be exactly a specific height.
	// Any Voxels above this height are turned to air.
	// Voxels below this height are brought up to be of the specified height, using a given Voxel as a "filler".
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	void FlattenRegionToHeight(const FRegion& Region, const int32 Height, UVoxel* Filler);

	// This changes a region to reflect a given heightmap.
	// This heightmap should be a 2D array, with dimensions the same as the width (x) and height (y) of the region being passed into it.
	// The Voxel passed into this function is used for any solid voxels in the heightmap.
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	void SetRegionHeightmap(const FRegion& Region, const TArray<float>& Heights, UVoxel* Filler);

	// This changes a region to reflect a given heightmap.
	// This heightmap should be a 2D array, with dimensions the same as the width (x) and height (y) of the region being passed into it.
	// The Voxel passed into this function is used for any solid voxels in the heightmap.
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	void SetRegionVoxels(const FRegion& Region, const TArray<float>& Heights, const TArray<uint8>& Materials);

	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	void SetHeightmapFromImage(UTexture2D* Texture, FIntVector StartingPoint, int32 RegionHeight, UVoxel* Filler);

	// This goes through an entire region and maps any solid voxels to the material ID for that point in the Materials array.
	// This heightmap should be a 2D array, with dimensions the same as the width (x) and height (y) of the region being passed into it.
	// Begin at Depth specifies how far beneath the surface the painting will start.
	// Penetrate Distance is how many voxels deep the material will extend below the surface.
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	void SetRegionMaterials(const FRegion& Region, const TArray<uint8>& Materials, int32 BeginAtDepth = 0, int32 PenetrateDistance = 256);

	UFUNCTION(BlueprintCallable, Category = "Volume|Debug")
	void DrawVolumeAsDebug(const FRegion& DebugRegion);
};
