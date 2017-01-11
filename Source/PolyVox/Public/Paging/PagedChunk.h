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

#include "UObject/NoExportTypes.h"
#include "RegionHelper.h"
#include "Pager.h"
#include "PagedChunk.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POLYVOX_API UPagedChunk : public UObject
{
	friend class APagedVolume;

	GENERATED_BODY()
public:
	~UPagedChunk();

	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void InitChunk(FVector Position, uint8 ChunkSideLength, UPager* VoxelPager = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void RemoveChunk();

	UFUNCTION(BlueprintPure, Category="Chunk|Voxels")
	TArray<UVoxel*> GetData() const;
	UFUNCTION(BlueprintPure, Category = "Chunk|Size")
	int32 GetDataSizeInBytes() const;

	UFUNCTION(BlueprintPure, Category = "Chunk|Voxels")
	UVoxel* GetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos);
	UFUNCTION(BlueprintPure, Category = "Chunk|Voxels")
	UVoxel* GetVoxelByVector(const FVector& Pos);

	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void SetVoxelFromCoordinates(int32 XPos, int32 YPos, int32 ZPos, UVoxel* Value);
	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void SetVoxelFromVector(const FVector& Pos, UVoxel* Value);
private:
	static int32 CalculateSizeInBytes(uint8 ChunkSideLength);

	// This is updated by the PagedVolume and used to discard the least recently used chunks.
	UPROPERTY()
	int32 ChunkLastAccessed;

	// This is so we can tell whether a uncompressed chunk has to be recompressed and whether
	// a compressed chunk has to be paged back to disk, or whether they can just be discarded.
	UPROPERTY()
	bool bDataModified;

	UPROPERTY()
	TArray<UVoxel*> VoxelData;
	UPROPERTY()
	uint8 SideLength;
	UPROPERTY()
	uint8 SideLengthPower;
	UPROPERTY()
	UPager* Pager;

	UPROPERTY()
	FVector ChunkSpacePosition;
};
