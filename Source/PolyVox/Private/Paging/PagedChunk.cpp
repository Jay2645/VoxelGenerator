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

#include "PolyVoxPrivatePCH.h"
#include "Utils/Morton.h"
#include "Mesh/VoxelProceduralMeshComponent.h"
#include "PagedChunk.h"

#define DO_CHECK = 1

APagedChunk::APagedChunk()
{
	VoxelMesh = CreateDefaultSubobject<UVoxelProceduralMeshComponent>(TEXT("Voxel Mesh Component"));
	RootComponent = VoxelMesh;
}

APagedChunk::~APagedChunk()
{
	if (VoxelData.Num() > 0)
	{
		RemoveChunk();
	}
}

void APagedChunk::RemoveChunk()
{
	if (bDataModified && Pager)
	{
		// From the coordinates of the chunk we deduce the coordinates of the contained voxels.
		FVector v3dLower = ChunkSpacePosition * (int32)(SideLength);
		FVector v3dUpper = v3dLower + FVector(SideLength - 1, SideLength - 1, SideLength - 1);

		// Page the data out
		//Pager->PageOut(ChunkRegion, this); // This is causing a crash on unload somehow

		bDataModified = false;
	}

	VoxelData.Empty();
}

void APagedChunk::InitChunk(FVector Position, uint8 ChunkSideLength, UPager* VoxelPager /*= nullptr*/)
{
	ChunkSpacePosition = Position;
	//SetActorLocation(Position * ChunkSideLength * 100.0f);
	SideLength = ChunkSideLength;
	SideLengthPower = FMath::Log2(SideLength);
	Pager = VoxelPager;
	bDataModified = true;
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
	FVector v3dUpper = v3dLower + FVector(SideLength, SideLength, SideLength);
	ChunkRegion = URegionHelper::CreateRegionFromVector(v3dLower, v3dUpper);

	FString chunkName = FString("Chunk (");
	chunkName += FString::FromInt((int32)v3dLower.X);
	chunkName += FString(", ");
	chunkName += FString::FromInt((int32)v3dLower.Y);
	chunkName += FString(", ");
	chunkName += FString::FromInt((int32)v3dLower.Z);
	chunkName += FString(") - (");
	chunkName += FString::FromInt((int32)v3dUpper.X);
	chunkName += FString(", ");
	chunkName += FString::FromInt((int32)v3dUpper.Y);
	chunkName += FString(", ");
	chunkName += FString::FromInt((int32)v3dUpper.Z);
	chunkName += FString(")");
	Rename(*chunkName);
	SetActorLabel(chunkName);
	// Page the data in
	Pager->PageIn(ChunkRegion, this);

	// We'll use this later to decide if data needs to be paged out again.
	bDataModified = false;
	bNeedsNewMarchingCubesMesh = true;
}

TArray<FVoxel> APagedChunk::GetData() const
{
	return VoxelData;
}

int32 APagedChunk::GetDataSizeInBytes() const
{
	return CalculateSizeInBytes(SideLength);
}

FVoxel APagedChunk::GetVoxelByCoordinatesWorldSpace(int32 XPos, int32 YPos, int32 ZPos)
{
	checkf(XPos >= ChunkRegion.LowerX, TEXT("Wrong chunk! Supplied x position %d is outside of the chunk boundaries %d"), XPos, ChunkRegion.LowerX);
	checkf(YPos >= ChunkRegion.LowerY, TEXT("Wrong chunk! Supplied y position %d is outside of the chunk boundaries %d"), YPos, ChunkRegion.LowerY);
	checkf(ZPos >= ChunkRegion.LowerZ, TEXT("Wrong chunk! Supplied z position %d is outside of the chunk boundaries %d"), ZPos, ChunkRegion.LowerZ);
	return GetVoxelByCoordinatesChunkSpace(XPos - ChunkRegion.LowerX, YPos - ChunkRegion.LowerY, ZPos - ChunkRegion.LowerZ);
}

FVoxel APagedChunk::GetVoxelByCoordinatesChunkSpace(int32 XPos, int32 YPos, int32 ZPos)
{
	// This code is not usually expected to be called by the user, with the exception of when implementing paging 
	// of uncompressed data. It's a performance critical code path so we use asserts rather than exceptions.
	checkf(XPos < SideLength, TEXT("Supplied x position %d is outside of the chunk boundaries %d"), XPos, SideLength);
	checkf(YPos < SideLength, TEXT("Supplied y position %d is outside of the chunk boundaries %d"), YPos, SideLength);
	checkf(ZPos < SideLength, TEXT("Supplied z position %d is outside of the chunk boundaries %d"), ZPos, SideLength);
	checkf(VoxelData.Num() > 0, TEXT("No uncompressed data - chunk must be decompressed before accessing voxels."));

	uint32 index = morton256_x[XPos] | morton256_y[YPos] | morton256_z[ZPos];

	checkf(index < (uint32)VoxelData.Num(), TEXT("Morton index %d out of bounds of voxel data size %d! Trying to access (%d, %d, %d)."), index, VoxelData.Num(), XPos, YPos, ZPos);

	return VoxelData[index];
}

void APagedChunk::SetVoxelByCoordinatesWorldSpace(int32 XPos, int32 YPos, int32 ZPos, FVoxel Value)
{
	checkf(XPos >= ChunkRegion.LowerX, TEXT("Wrong chunk! Supplied x position %d is outside of the chunk boundaries %d"), XPos, ChunkRegion.LowerX);
	checkf(YPos >= ChunkRegion.LowerY, TEXT("Wrong chunk! Supplied y position %d is outside of the chunk boundaries %d"), YPos, ChunkRegion.LowerY);
	checkf(ZPos >= ChunkRegion.LowerZ, TEXT("Wrong chunk! Supplied z position %d is outside of the chunk boundaries %d"), ZPos, ChunkRegion.LowerZ);
	SetVoxelByCoordinatesChunkSpace(XPos - ChunkRegion.LowerX, YPos - ChunkRegion.LowerY, ZPos - ChunkRegion.LowerZ, Value);
}

void APagedChunk::SetVoxelByCoordinatesChunkSpace(int32 XPos, int32 YPos, int32 ZPos, FVoxel Value)
{
	// This code is not usually expected to be called by the user, with the exception of when implementing paging 
	// of uncompressed data. It's a performance critical code path so we use asserts rather than exceptions.
	checkf(XPos < SideLength, TEXT("Supplied x position %d is outside of the chunk boundaries %d"), XPos, SideLength);
	checkf(YPos < SideLength, TEXT("Supplied y position %d is outside of the chunk boundaries %d"), YPos, SideLength);
	checkf(ZPos < SideLength, TEXT("Supplied z position %d is outside of the chunk boundaries %d"), ZPos, SideLength);
	checkf(VoxelData.Num() > 0, TEXT("No uncompressed data - chunk must be decompressed before accessing voxels."));

	uint32 index = morton256_x[XPos] | morton256_y[YPos] | morton256_z[ZPos];

	checkf(index < (uint32)VoxelData.Num(), TEXT("Morton index %d out of bounds of voxel data size %d! Trying to access (%d, %d, %d)."), index, VoxelData.Num(), XPos, YPos, ZPos);

	VoxelData[index] = Value;

	bDataModified = true;
	bNeedsNewMarchingCubesMesh = true;
}

void APagedChunk::CreateMarchingCubesMesh(ABaseVolume* Volume, TArray<FVoxelMaterial> VoxelMaterials)
{
	if(bNeedsNewMarchingCubesMesh)
	{
		UE_LOG(LogPolyVox, Log, TEXT("Creating PolyVox mesh for %s, region (%d, %d, %d) to (%d, %d, %d)"), *GetName(), ChunkRegion.LowerX, ChunkRegion.LowerY, ChunkRegion.LowerZ, ChunkRegion.UpperX, ChunkRegion.UpperY, ChunkRegion.UpperZ);
		VoxelMesh->CreateMarchingCubesMesh(Volume, ChunkRegion, VoxelMaterials);
	}
	bNeedsNewMarchingCubesMesh = false;
}

FVoxel APagedChunk::GetDataAtIndex(const int32 CurrentVoxelIndex) const
{
	if (CurrentVoxelIndex < 0 || CurrentVoxelIndex >= VoxelData.Num())
	{
		UE_LOG(LogPolyVox, Warning, TEXT("Current voxel index %d was out of range!"), CurrentVoxelIndex);
		return FVoxel::GetEmptyVoxel();
	}
	return VoxelData[CurrentVoxelIndex];
}

int32 APagedChunk::CalculateSizeInBytes(uint8 ChunkSideLength)
{
	// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
	// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
	return ChunkSideLength * ChunkSideLength * ChunkSideLength * sizeof(FVoxel);
}
