// Fill out your copyright notice in the Description page of Project Settings.

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
	TArray<FVoxel> GetData() const;
	UFUNCTION(BlueprintPure, Category = "Chunk|Size")
	int32 GetDataSizeInBytes() const;

	UFUNCTION(BlueprintPure, Category = "Chunk|Voxels")
	FVoxel GetVoxelFromCoordinates(int32 XPos, int32 YPos, int32 ZPos) const;
	UFUNCTION(BlueprintPure, Category = "Chunk|Voxels")
	FVoxel GetVoxelFromVector(const FVector& Pos) const;

	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void SetVoxelFromCoordinates(int32 XPos, int32 YPos, int32 ZPos, FVoxel Value);
	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void SetVoxelFromVector(const FVector& Pos, FVoxel Value);
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
	TArray<FVoxel> VoxelData;
	UPROPERTY()
	uint8 SideLength;
	UPROPERTY()
	uint8 SideLengthPower;
	UPROPERTY()
	UPager* Pager;

	UPROPERTY()
	FVector ChunkSpacePosition;
};
