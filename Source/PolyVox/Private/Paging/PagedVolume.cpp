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
#include "PagedVolume.h"

APagedVolume::APagedVolume()
{
	ArrayChunks.SetNumZeroed(CHUNK_ARRAY_SIZE);
}

APagedVolume::~APagedVolume()
{
	//FlushAll();
}

void APagedVolume::InitializeVolume(TSubclassOf<UPager> VolumePager, int32 TargetMemoryUsageInBytes /*= 256 * 1024 * 1024*/, uint8 VolumeChunkSideLength /*= 32*/)
{
	if (VolumePager == NULL)
	{
		UE_LOG(LogPolyVox, Fatal, TEXT("A valid pager must be specified to use a PagedVolume."));
		return;
	}
	if (TargetMemoryUsageInBytes < 1 * 1024 * 1024)
	{
		UE_LOG(LogPolyVox, Fatal, TEXT("Target memory usage is too small to be practical."));
		return;
	}
	if (VolumeChunkSideLength == 0)
	{
		UE_LOG(LogPolyVox, Fatal, TEXT("Chunk size cannot be 0!"));
		return;
	}
	else if (VolumeChunkSideLength != 2 && VolumeChunkSideLength != 4 && VolumeChunkSideLength != 8 && VolumeChunkSideLength != 16 && VolumeChunkSideLength != 32 && VolumeChunkSideLength != 64 && VolumeChunkSideLength != 128)
	{
		UE_LOG(LogPolyVox, Fatal, TEXT("Chunk size must be a power of 2."));
		return;
	}

	ChunkSideLength = VolumeChunkSideLength;
	// Used to perform multiplications and divisions by bit shifting.
	ChunkSideLengthPower = (uint8)FMath::Log2(ChunkSideLength);
	// Use to perform modulo by bit operations
	ChunkMask = ChunkSideLength - 1;

	// Calculate the number of chunks based on the memory limit and the size of each chunk.
	int32 ChunkSizeInBytes = UPagedChunk::CalculateSizeInBytes(ChunkSideLength);
	ChunkCountLimit = TargetMemoryUsageInBytes / ChunkSizeInBytes;

	// Enforce sensible limits on the number of chunks.
	const int32 MinPracticalNoOfChunks = 32; // Enough to make sure a chunks and it's neighbors can be loaded, with a few to spare.
	const int32 MaxPracticalNoOfChunks = CHUNK_ARRAY_SIZE / 2; // A hash table should only become half-full to avoid too many clashes.
	if(ChunkCountLimit < MinPracticalNoOfChunks)
	{
		UE_LOG(LogPolyVox, Warning, TEXT("Requested memory usage limit of %d MB is too low and cannot be adhered to."), (TargetMemoryUsageInBytes / (1024 * 1024)));
	}
	ChunkCountLimit = FMath::Max(ChunkCountLimit, MinPracticalNoOfChunks);
	ChunkCountLimit = FMath::Min(ChunkCountLimit, MaxPracticalNoOfChunks);

	Pager = NewObject<UPager>((UObject*)GetTransientPackage(), VolumePager,NAME_None);

	// Inform the user about the chosen memory configuration.
	UE_LOG(LogPolyVox, Log, TEXT("Memory usage limit for volume now set to %d MB (%d chunks of %d KB each)."), ((ChunkCountLimit * ChunkSizeInBytes) / (1024 * 1024)), ChunkCountLimit, (ChunkSizeInBytes / 1024));
}

UVoxel* APagedVolume::GetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos)
{
	const int32 chunkX = XPos >> ChunkSideLengthPower;
	const int32 chunkY = YPos >> ChunkSideLengthPower;
	const int32 chunkZ = ZPos >> ChunkSideLengthPower;

	const uint16 xOffset = (uint16)(XPos & ChunkMask);
	const uint16 yOffset = (uint16)(YPos & ChunkMask);
	const uint16 zOffset = (uint16)(ZPos & ChunkMask);

	auto pChunk = CanReuseLastAccessedChunk(chunkX, chunkY, chunkZ) ? LastAccessedChunk : GetChunk(chunkX, chunkY, chunkZ);

	return pChunk->GetVoxelByCoordinates(xOffset, yOffset, zOffset);
}

UVoxel* APagedVolume::GetVoxelByVector(const FVector& Coordinates)
{
	return GetVoxelByCoordinates((int32)Coordinates.X, (int32)Coordinates.Y, (int32)Coordinates.Z);
}

void APagedVolume::SetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos, UVoxel* Voxel)
{
	const int32 chunkX = XPos >> ChunkSideLengthPower;
	const int32 chunkY = YPos >> ChunkSideLengthPower;
	const int32 chunkZ = ZPos >> ChunkSideLengthPower;

	const uint16 xOffset = (uint16)(XPos & ChunkMask);
	const uint16 yOffset = (uint16)(YPos & ChunkMask);
	const uint16 zOffset = (uint16)(ZPos & ChunkMask);

	auto pChunk = CanReuseLastAccessedChunk(chunkX, chunkY, chunkZ) ? LastAccessedChunk : GetChunk(chunkX, chunkY, chunkZ);

	pChunk->SetVoxelFromCoordinates(xOffset, yOffset, zOffset, Voxel);
}

void APagedVolume::SetVoxelByVector(const FVector& Coordinates, UVoxel* Voxel)
{
	SetVoxelByCoordinates((int32)Coordinates.X, (int32)Coordinates.Y, (int32)Coordinates.Z, Voxel);
}

void APagedVolume::Prefetch(FRegion PrefetchRegion)
{
	// Convert the start and end positions into chunk space coordinates
	FVector lowerCorner = URegionHelper::GetLowerCorner(PrefetchRegion);
	FVector start;
	start.X = ((int32)lowerCorner.X) >> ChunkSideLengthPower;
	start.Y = ((int32)lowerCorner.Y) >> ChunkSideLengthPower;
	start.Z = ((int32)lowerCorner.Z) >> ChunkSideLengthPower;

	FVector upperCorner = URegionHelper::GetUpperCorner(PrefetchRegion);
	FVector end;
	end.X = ((int32)upperCorner.X) >> ChunkSideLengthPower;
	end.Y = ((int32)upperCorner.Y) >> ChunkSideLengthPower;
	end.Z = ((int32)upperCorner.Z) >> ChunkSideLengthPower;

	// Ensure we don't page in more chunks than the volume can hold.
	FRegion region = URegionHelper::CreateRegionFromVector(start, end);
	int32 noOfChunks = (URegionHelper::GetWidthInVoxels(region) * URegionHelper::GetHeightInVoxels(region) * URegionHelper::GetDepthInVoxels(region));
	if (noOfChunks > ChunkCountLimit)
	{
		UE_LOG(LogPolyVox, Warning, TEXT("Attempting to prefetch more than the maximum number of chunks (this will cause thrashing)."));
	}
	noOfChunks = FMath::Min(noOfChunks, ChunkCountLimit);

	// Loops over the specified positions and touch the corresponding chunks.
	for (int32 x = start.X; x <= end.X; x++)
	{
		for (int32 y = start.Y; y <= end.Y; y++)
		{
			for (int32 z = start.Z; z <= end.Z; z++)
			{
				GetChunk(x, y, z);
			}
		}
	}
}

void APagedVolume::FlushAll()
{
	// Clear this pointer as all chunks are about to be removed.
	LastAccessedChunk = NULL;

	// Erase all the most recently used chunks.
	for (uint32 uIndex = 0; uIndex < CHUNK_ARRAY_SIZE; uIndex++)
	{
		UPagedChunk* chunk = ArrayChunks[uIndex];
		if (chunk != NULL)
		{
			chunk->RemoveChunk();
		}
	}
	ArrayChunks.Empty();
}

int32 APagedVolume::CalculateSizeInBytes() const
{
	uint32_t uChunkCount = 0;
	for (uint32_t uIndex = 0; uIndex < CHUNK_ARRAY_SIZE; uIndex++)
	{
		if (ArrayChunks[uIndex])
		{
			uChunkCount++;
		}
	}

	// Note: We disregard the size of the other class members as they are likely to be very small compared to the size of the
	// allocated voxel data. This also keeps the reported size as a power of two, which makes other memory calculations easier.
	return UPagedChunk::CalculateSizeInBytes(ChunkSideLength) * uChunkCount;
}

uint8 APagedVolume::GetSideLengthPower() const
{
	return ChunkSideLengthPower;
}

bool APagedVolume::CanReuseLastAccessedChunk(int32 ChunkX, int32 ChunkY, int32 ChunkZ) const
{
	return ((ChunkX == LastAccessedChunkX) &&
			(ChunkY == LastAccessedChunkY) &&
			(ChunkZ == LastAccessedChunkZ) &&
			(LastAccessedChunk != NULL));
}

UPagedChunk* APagedVolume::GetChunk(int32 ChunkX, int32 ChunkY, int32 ChunkZ)
{
	UPagedChunk* chunk = nullptr;

	// Extract the lower five bits from each position component.
	const uint32 chunkXLowerBits = static_cast<uint32>(ChunkX & 0x1F);
	const uint32 chunkYLowerBits = static_cast<uint32>(ChunkY & 0x1F);
	const uint32 chunkZLowerBits = static_cast<uint32>(ChunkZ & 0x1F);
	// Combine then to form a 15-bit hash of the position. Also shift by one to spread the values out in the whole 16-bit space.
	const uint32 posisionHash = (((chunkXLowerBits)) | ((chunkYLowerBits) << 5) | ((chunkZLowerBits) << 10) << 1);

	// Starting at the position indicated by the hash, and then search through the whole array looking for a chunk with the correct
	// position. In most cases we expect to find it in the first place we look. Note that this algorithm is slow in the case that
	// the chunk is not found because the whole array has to be searched, but in this case we are going to have to page the data in
	// from an external source which is likely to be slow anyway.
	uint32 iIndex = posisionHash;
	do
	{
		if (ArrayChunks[iIndex] != NULL)
		{
			FVector& entryPos = ArrayChunks[iIndex]->ChunkSpacePosition;
			if (entryPos.X == ChunkX && entryPos.Y == ChunkY && entryPos.Z == ChunkZ)
			{
				chunk = ArrayChunks[iIndex];
				Timestamper++;
				chunk->ChunkLastAccessed = Timestamper;
				break;
			}
		}

		iIndex++;
		iIndex %= CHUNK_ARRAY_SIZE;
	} while (iIndex != posisionHash); // Keep searching until we get back to our start position.

	// If we still haven't found the chunk then it's time to create a new one and page it in from disk.
	if (chunk == NULL)
	{
		// The chunk was not found so we will create a new one.
		FVector chunkPos(ChunkX, ChunkY, ChunkZ);
		chunk = NewObject<UPagedChunk>();
		chunk->InitChunk(chunkPos,ChunkSideLength,Pager);
		Timestamper++;
		chunk->ChunkLastAccessed = Timestamper; // Important, as we may soon delete the oldest chunk

												 // Store the chunk at the appropriate place in out chunk array. Ideally this place is
												 // given by the hash, otherwise we do a linear search for the next available location
												 // We always expect to find a free place because we aim to keep the array only half full.
		iIndex = posisionHash;
		bool bInsertedSucessfully = false;
		do
		{
			if (ArrayChunks[iIndex] == NULL)
			{
				ArrayChunks[iIndex] = chunk;
				bInsertedSucessfully = true;
				break;
			}

			iIndex++;
			iIndex %= CHUNK_ARRAY_SIZE;
		} while (iIndex != posisionHash); // Keep searching until we get back to our start position.

										   // This should never really happen unless we are failing to keep our number of active chunks
										   // significantly under the target amount. Perhaps if chunks are 'pinned' for threading purposes?
		if(!bInsertedSucessfully)
		{
			UE_LOG(LogPolyVox, Fatal, TEXT("No space in chunk array for new chunk."));
			return NULL;
		}
		// As we have added a chunk we may have exceeded our target chunk limit. Search through the array to
		// determine how many chunks we have, as well as finding the oldest timestamp. Note that this is potentially
		// wasteful and we may instead wish to track how many chunks we have and/or delete a chunk at random (or
		// just check e.g. 10 and delete the oldest of those) but we'll see if this is a bottleneck first. Paging
		// the data in is probably more expensive.
		int32 chunkCount = 0;
		int32 oldestChunkIndex = 0;
		int32 oldestChunkTimestamp = 2147483647;
		for (uint32 uIndex = 0; uIndex < CHUNK_ARRAY_SIZE; uIndex++)
		{
			if (ArrayChunks[uIndex])
			{
				chunkCount++;
				if (ArrayChunks[uIndex]->ChunkLastAccessed < oldestChunkTimestamp)
				{
					oldestChunkTimestamp = ArrayChunks[uIndex]->ChunkLastAccessed;
					oldestChunkIndex = uIndex;
				}
			}
		}

		// Check if we have too many chunks, and delete the oldest if so.
		if (chunkCount > ChunkCountLimit)
		{
			ArrayChunks[oldestChunkIndex] = NULL;
		}
	}

	LastAccessedChunk = chunk;
	LastAccessedChunkX = ChunkX;
	LastAccessedChunkY = ChunkY;
	LastAccessedChunkZ = ChunkZ;

	return chunk;
}
