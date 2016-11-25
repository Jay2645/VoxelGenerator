// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "VoxelHelper.h"
#include "VoxelHeightmap.generated.h"

/**
 * 
 */
UCLASS()
class POLYVOX_API UVoxelHeightmap : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Heightmap")
	FVoxelHeightmapData GetMapPoint(int32 PointX, int32 PointY) const;
};
