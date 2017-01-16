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
	FVector v3dUpper = v3dLower + FVector(SideLength, SideLength, SideLength);
	ChunkRegion = URegionHelper::CreateRegionFromVector(v3dLower, v3dUpper);
	//FString chunkName = TEXT("Chunk ("+ChunkRegion.LowerX+", "+ChunkRegion.LowerY+", "+ChunkRegion.LowerZ+") - ("+ChunkRegion.UpperX+", "+ChunkRegion.UpperY+", "+ChunkRegion.UpperZ+")";
	//Rename(TEXT(chunkName));
	// Page the data in
	Pager->PageIn(ChunkRegion, this);

	// We'll use this later to decide if data needs to be paged out again.
	bDataModified = false;
	bNeedsNewMarchingCubesMesh = true;
}

TArray<UVoxel*> APagedChunk::GetData() const
{
	return VoxelData;
}

int32 APagedChunk::GetDataSizeInBytes() const
{
	return CalculateSizeInBytes(SideLength);
}

UVoxel* APagedChunk::GetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos)
{
	// This code is not usually expected to be called by the user, with the exception of when implementing paging 
	// of uncompressed data. It's a performance critical code path so we use asserts rather than exceptions.
	checkf(XPos < SideLength, TEXT("Supplied position %d is outside of the chunk boundaries %d"), XPos, SideLength);
	checkf(YPos < SideLength, TEXT("Supplied position %d is outside of the chunk boundaries %d"), YPos, SideLength);
	checkf(ZPos < SideLength, TEXT("Supplied position %d is outside of the chunk boundaries %d"), ZPos, SideLength);
	checkf(VoxelData.Num() > 0, TEXT("No uncompressed data - chunk must be decompressed before accessing voxels."));

	uint32_t index = morton256_x[XPos] | morton256_y[YPos] | morton256_z[ZPos];

	if (VoxelData[index] == NULL)
	{
		VoxelData[index] = UVoxel::GetEmptyVoxel();
	}
	return VoxelData[index];
}

UVoxel* APagedChunk::GetVoxelByVector(const FVector& Pos)
{
	return GetVoxelByCoordinates((int32)Pos.X, (int32)Pos.Y, (int32)Pos.Z);
}

void APagedChunk::SetVoxelFromCoordinates(int32 XPos, int32 YPos, int32 ZPos, UVoxel* Value)
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
	bNeedsNewMarchingCubesMesh = true;
}

void APagedChunk::SetVoxelFromVector(const FVector& Pos, UVoxel* Value)
{
	SetVoxelFromCoordinates((int32)Pos.X, (int32)Pos.Y, (int32)Pos.Z, Value);
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

int32 APagedChunk::CalculateSizeInBytes(uint8 ChunkSideLength)
{
	// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
	// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
	return ChunkSideLength * ChunkSideLength * ChunkSideLength * sizeof(UVoxel);
}
