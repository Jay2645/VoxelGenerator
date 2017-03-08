// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Paging/Pager.h"
#include "FlatPager.generated.h"

/**
 * Generates a perfectly flat plane at the level the user specifies.
 * This is additive, not reductive, so any voxels above the level of the plane are unaffected.
 */
UCLASS(Blueprintable)
class POLYVOX_API UFlatPager : public UPager
{
	GENERATED_BODY()
public:
	// How high up the ground should be, in voxel space.
	// Everything below this will be set to solid.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	int32 GroundLevel;
	// The material to use for the voxel.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	uint8 VoxelMaterial;

protected:
	virtual void PageIn_Implementation(const FRegion& Region, APagedChunk* Chunk) override;
};
