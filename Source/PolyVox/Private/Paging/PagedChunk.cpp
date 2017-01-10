// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "Utils/Morton.h"
#include "PagedChunk.h"

#define DO_CHECK = 1

UPagedChunk::~UPagedChunk()
{
	if (VoxelData.Num() > 0)
	{
		RemoveChunk();
	}
}

void UPagedChunk::RemoveChunk()
{
	if (bDataModified && Pager)
	{
		// From the coordinates of the chunk we deduce the coordinates of the contained voxels.
		FVector v3dLower = ChunkSpacePosition * (int32)(SideLength);
		FVector v3dUpper = v3dLower + FVector(SideLength - 1, SideLength - 1, SideLength - 1);

		// Page the data out
		//Pager->PageOut(URegionHelper::CreateRegionFromVector(v3dLower, v3dUpper), this); // This is causing a crash on unload somehow

		bDataModified = false;
	}

	VoxelData.Empty();
}

void UPagedChunk::InitChunk(FVector Position, uint8 ChunkSideLength, UPager* VoxelPager /*= nullptr*/)
{
	ChunkSpacePosition = Position;
	SideLength = ChunkSideLength;
	SideLengthPower = FMath::Log2(SideLength);
	Pager = VoxelPager;
	bDataModified = true;
	ChunkLastAccessed = 0;
	VoxelData.Empty();

	if (Pager == NULL)
	{
		UE_LOG(LogPolyVox, Fatal, TEXT("No pager was given to the chunk!"));
		return;
	}

	// Allocate the data
	VoxelData.AddDefaulted(SideLength * SideLength * SideLength);

	// Pass the chunk to the Pager to give it a chance to initialize it with any data
	// From the coordinates of the chunk we deduce the coordinates of the contained voxels.
	FVector v3dLower = ChunkSpacePosition * (int32)(SideLength);
	FVector v3dUpper = v3dLower + FVector(SideLength - 1, SideLength - 1, SideLength - 1);
	FRegion reg = URegionHelper::CreateRegionFromVector(v3dLower, v3dUpper);

	// Page the data in
	Pager->PageIn(reg, this);

	// We'll use this later to decide if data needs to be paged out again.
	bDataModified = false;
}

TArray<FVoxel> UPagedChunk::GetData() const
{
	return VoxelData;
}

int32 UPagedChunk::GetDataSizeInBytes() const
{
	return CalculateSizeInBytes(SideLength);
}

FVoxel UPagedChunk::GetVoxelFromCoordinates(int32 XPos, int32 YPos, int32 ZPos) const
{
	// This code is not usually expected to be called by the user, with the exception of when implementing paging 
	// of uncompressed data. It's a performance critical code path so we use asserts rather than exceptions.
	checkf(XPos < SideLength, TEXT("Supplied position %d is outside of the chunk boundaries %d"), XPos, SideLength);
	checkf(YPos < SideLength, TEXT("Supplied position %d is outside of the chunk boundaries %d"), YPos, SideLength);
	checkf(ZPos < SideLength, TEXT("Supplied position %d is outside of the chunk boundaries %d"), ZPos, SideLength);
	checkf(VoxelData.Num() > 0, TEXT("No uncompressed data - chunk must be decompressed before accessing voxels."));

	uint32_t index = morton256_x[XPos] | morton256_y[YPos] | morton256_z[ZPos];

	return VoxelData[index];
}

FVoxel UPagedChunk::GetVoxelFromVector(const FVector& Pos) const
{
	return GetVoxelFromCoordinates((int32)Pos.X, (int32)Pos.Y, (int32)Pos.Z);
}

void UPagedChunk::SetVoxelFromCoordinates(int32 XPos, int32 YPos, int32 ZPos, FVoxel Value)
{
	// This code is not usually expected to be called by the user, with the exception of when implementing paging 
	// of uncompressed data. It's a performance critical code path so we use asserts rather than exceptions.
	checkf(XPos < SideLength, TEXT("Supplied position %d is outside of the chunk boundaries %d"), XPos, SideLength);
	checkf(YPos < SideLength, TEXT("Supplied position %d is outside of the chunk boundaries %d"), YPos, SideLength);
	checkf(ZPos < SideLength, TEXT("Supplied position %d is outside of the chunk boundaries %d"), ZPos, SideLength);
	checkf(VoxelData.Num() > 0, TEXT("No uncompressed data - chunk must be decompressed before accessing voxels."));

	uint32_t index = morton256_x[XPos] | morton256_y[YPos] | morton256_z[ZPos];

	VoxelData[index] = Value;

	bDataModified = true;
}

void UPagedChunk::SetVoxelFromVector(const FVector& Pos, FVoxel Value)
{
	SetVoxelFromCoordinates((int32)Pos.X, (int32)Pos.Y, (int32)Pos.Z, Value);
}

int32 UPagedChunk::CalculateSizeInBytes(uint8 ChunkSideLength)
{
	// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
	// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
	return ChunkSideLength * ChunkSideLength * ChunkSideLength * sizeof(FVoxel);
}
