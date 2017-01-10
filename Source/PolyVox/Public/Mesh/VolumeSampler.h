// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "RegionHelper.h"
#include "Paging/PagedChunk.h"
#include "BaseVolume.h"
#include "VolumeSampler.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POLYVOX_API UVolumeSampler : public UObject
{
	GENERATED_BODY()
public:
	void Initalize(ABaseVolume* VolumeData);
	FVoxel GetVoxel();
	void SetPosition(int32 XPos, int32 YPos, int32 ZPos);
	void MoveNegativeX();
	void MovePositiveX();
	void MoveNegativeY();
	void MovePositiveY();
	void MoveNegativeZ();
	void MovePositiveZ();

private:
	UPROPERTY()
	ABaseVolume* Volume;

	//The current position in the volume
	UPROPERTY()
	int32 XPosInVolume;
	UPROPERTY()
	int32 YPosInVolume;
	UPROPERTY()
	int32 ZPosInVolume;

	UPROPERTY()
	int32 CurrentVoxelIndex;
	UPROPERTY()
	UPagedChunk* CurrentChunk;

	UPROPERTY()
	int32 XPosInChunk;
	UPROPERTY()
	int32 YPosInChunk;
	UPROPERTY()
	int32 ZPosInChunk;

	uint16 ChunkSideLengthMinusOne;
};
