// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "VoxelTerrainPager.h"
#include "Chunk.h"
#include "Region.h"
#include "ChunkManager.h"
#include "Pager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class POLYVOX_API UPager : public UObject
{
	GENERATED_BODY()

private:
	TSharedPtr<VoxelTerrainPager> VoxelPager;
protected:
	UChunkManager* ChunkManager;

	virtual void InitializePager_Implementation(UChunkManager* Manager);

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Pager")
	void InitializePager(UChunkManager* Manager);

	VoxelTerrainPager* GetPager() const;
protected:
	virtual void PageIn_Implementation(URegion* VoxelRegion, UChunk* Chunk);
	virtual void PageOut_Implementation(URegion* VoxelRegion, UChunk* Chunk);
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Chunks|Paging")
	void PageIn(URegion* VoxelRegion, UChunk* Chunk);
	UFUNCTION(BlueprintNativeEvent, Category = "Chunks|Paging")
	void PageOut(URegion* VoxelRegion, UChunk* Chunk);

protected:
	virtual FVoxelHeightmapData GetPointHeightmap_Implementation(URegion* VoxelRegion, int32 PointX, int32 PointY);
	virtual void ProcessPoint_Implementation(URegion* VoxelRegion, UChunk* Chunk, int32 PointX, int32 PointY);
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Chunks|Paging")
	FVoxelHeightmapData GetPointHeightmap(URegion* VoxelRegion, int32 PointX, int32 PointY);
	UFUNCTION(BlueprintNativeEvent, Category = "Chunks|Paging")
	void ProcessPoint(URegion* VoxelRegion, UChunk* Chunk, int32 PointX, int32 PointY);
};