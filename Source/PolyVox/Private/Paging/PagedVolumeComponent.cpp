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
#include "PagedChunk.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
#include "Mesh/VoxelProceduralMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Utils/ArrayHelper.h"
#include "PagedVolumeComponent.h"


// Sets default values for this component's properties
UPagedVolumeComponent::UPagedVolumeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ArrayChunks.SetNumZeroed(CHUNK_ARRAY_SIZE);
	VolumePager = UPager::StaticClass();
	ChunkSideLength = 32;
}

UPagedVolumeComponent::~UPagedVolumeComponent()
{
	//FlushAll();
}


// Called when the game starts
void UPagedVolumeComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeVolume(VolumePager, TargetMemoryUsageInBytes, ChunkSideLength);
}


// Called every frame
void UPagedVolumeComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (ChunkCurrentlyMakingMeshFor != NULL)
	{
		// We are working on making a chunk
		if (!ChunkCurrentlyMakingMeshFor->bNeedsNewMarchingCubesMesh)
		{
			// Chunk is done being created; we don't need to work on the chunk anymore
			ChunkCurrentlyMakingMeshFor = NULL;
		}
		else
		{
			// Chunk is still being worked on; wait a frame
			return;
		}
	}
	// At this point, we are no longer working on any chunk
	if (ChunksToCreateMesh.IsEmpty())
	{
		// We have no chunks to make
		return;
	}
	else
	{
		ChunksToCreateMesh.Dequeue(ChunkCurrentlyMakingMeshFor);
		if (ChunkCurrentlyMakingMeshFor != NULL)
		{
			ChunkCurrentlyMakingMeshFor->CreateMarchingCubesMesh(this, ChunkMaterials);
		}
	}
}


void UPagedVolumeComponent::InitializeVolume(TSubclassOf<UPager> PagerClass, int32 MemoryUsageInBytes /*= 256 * 1024 * 1024*/, uint8 VolumeChunkSideLength /*= 32*/)
{
	if (PagerClass == NULL)
	{
		UE_LOG(LogPolyVox, Fatal, TEXT("A valid pager must be specified to use a PagedVolume."));
		return;
	}
	if (MemoryUsageInBytes < 1 * 1024 * 1024)
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

	if (ArrayChunks.Num() == 0)
	{
		ArrayChunks.SetNumZeroed(CHUNK_ARRAY_SIZE);
	}

	ChunkSideLength = VolumeChunkSideLength;
	// Used to perform multiplications and divisions by bit shifting.
	ChunkSideLengthPower = (uint8)FMath::Log2(ChunkSideLength);
	// Use to perform modulo by bit operations
	ChunkMask = ChunkSideLength - 1;

	// Calculate the number of chunks based on the memory limit and the size of each chunk.
	int32 ChunkSizeInBytes = APagedChunk::CalculateSizeInBytes(ChunkSideLength);
	ChunkCountLimit = MemoryUsageInBytes / ChunkSizeInBytes;

	// Enforce sensible limits on the number of chunks.
	const int32 MinPracticalNoOfChunks = 32; // Enough to make sure a chunks and it's neighbors can be loaded, with a few to spare.
	const int32 MaxPracticalNoOfChunks = CHUNK_ARRAY_SIZE / 2; // A hash table should only become half-full to avoid too many clashes.
	if (ChunkCountLimit < MinPracticalNoOfChunks)
	{
		UE_LOG(LogPolyVox, Warning, TEXT("Requested memory usage limit of %d MB is too low and cannot be adhered to."), (MemoryUsageInBytes / (1024 * 1024)));
	}
	ChunkCountLimit = FMath::Max(ChunkCountLimit, MinPracticalNoOfChunks);
	ChunkCountLimit = FMath::Min(ChunkCountLimit, MaxPracticalNoOfChunks);
	
	VolumePager = PagerClass;
	Pager = NewObject<UPager>((UObject*)GetTransientPackage(), PagerClass, NAME_None);

	// Inform the user about the chosen memory configuration.
	UE_LOG(LogPolyVox, Log, TEXT("Memory usage limit for volume now set to %d MB (%d chunks of %d KB each)."), ((ChunkCountLimit * ChunkSizeInBytes) / (1024 * 1024)), ChunkCountLimit, (ChunkSizeInBytes / 1024));
}

FVoxel UPagedVolumeComponent::GetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos)
{
	const int32 chunkX = XPos >> ChunkSideLengthPower;
	const int32 chunkY = YPos >> ChunkSideLengthPower;
	const int32 chunkZ = ZPos >> ChunkSideLengthPower;

	const uint16 xOffset = (uint16)(XPos & ChunkMask);
	const uint16 yOffset = (uint16)(YPos & ChunkMask);
	const uint16 zOffset = (uint16)(ZPos & ChunkMask);

	auto pChunk = CanReuseLastAccessedChunk(chunkX, chunkY, chunkZ) ? LastAccessedChunk : GetChunk(chunkX, chunkY, chunkZ);

	return pChunk->GetVoxelByCoordinatesChunkSpace(xOffset, yOffset, zOffset);
}

FVoxel UPagedVolumeComponent::GetVoxelByVector(const FVector& Coordinates)
{
	return GetVoxelByCoordinates((int32)Coordinates.X, (int32)Coordinates.Y, (int32)Coordinates.Z);
}

void UPagedVolumeComponent::SetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos, FVoxel Voxel)
{
	const int32 chunkX = XPos >> ChunkSideLengthPower;
	const int32 chunkY = YPos >> ChunkSideLengthPower;
	const int32 chunkZ = ZPos >> ChunkSideLengthPower;

	const uint16 xOffset = (uint16)(XPos & ChunkMask);
	const uint16 yOffset = (uint16)(YPos & ChunkMask);
	const uint16 zOffset = (uint16)(ZPos & ChunkMask);

	auto pChunk = CanReuseLastAccessedChunk(chunkX, chunkY, chunkZ) ? LastAccessedChunk : GetChunk(chunkX, chunkY, chunkZ);

	pChunk->SetVoxelByCoordinatesChunkSpace(xOffset, yOffset, zOffset, Voxel);
}

void UPagedVolumeComponent::SetVoxelByVector(const FVector& Coordinates, FVoxel Voxel)
{
	SetVoxelByCoordinates((int32)Coordinates.X, (int32)Coordinates.Y, (int32)Coordinates.Z, Voxel);
}

void UPagedVolumeComponent::PageInChunksAroundPlayer(AController* PlayerController, const int32 MaxWorldHeight, const uint8 NumberOfChunksToPageIn, TArray<FVoxelMaterial> Materials, bool bUseMarchingCubes)
{
	if (PlayerController == NULL)
	{
		return;
	}

	APawn* playerPawn = PlayerController->GetPawn();
	FVector regionCenter = playerPawn->GetActorLocation();
	regionCenter.X /= VoxelSize;
	regionCenter.Y /= VoxelSize;
	regionCenter.Z = 0.0f;
	FVector regionExtents = FVector(NumberOfChunksToPageIn * ChunkSideLength, NumberOfChunksToPageIn * ChunkSideLength, MaxWorldHeight);
	FRegion pageInRegion = URegionHelper::CreateRegionFromVector(regionCenter - regionExtents, regionCenter + regionExtents);
	UE_LOG(LogPolyVox, Log, TEXT("Paging in %d chunks around player position (%f, %f, %f), creating a region (%d, %d, %d) to (%d, %d, %d)."), NumberOfChunksToPageIn, regionCenter.X, regionCenter.Y, regionCenter.Z, pageInRegion.LowerX, pageInRegion.LowerY, pageInRegion.LowerZ, pageInRegion.UpperX, pageInRegion.UpperY, pageInRegion.UpperZ);
	if (bUseMarchingCubes)
	{
		CreateMarchingCubesMesh(pageInRegion, Materials);
	}
	else
	{
		unimplemented();
	}
}

TArray<APagedChunk*> UPagedVolumeComponent::Prefetch(FRegion PrefetchRegion)
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

	UE_LOG(LogPolyVox, Log, TEXT("Fetching chunks from (%f, %f, %f) to (%f, %f, %f)."), start.X, start.Y, start.Z, end.X, end.Y, end.Z);

	// Loops over the specified positions and touch the corresponding chunks.
	TArray<APagedChunk*> touchedChunks;
	for (int32 x = start.X; x <= end.X; x++)
	{
		for (int32 y = start.Y; y <= end.Y; y++)
		{
			for (int32 z = start.Z; z <= end.Z; z++)
			{
				touchedChunks.Add(GetChunk(x, y, z));
			}
		}
	}

	// As we have added a chunk we may have exceeded our target chunk limit. Search through the array to
	// determine how many chunks we have, as well as finding the oldest timestamp. Note that this is potentially
	// wasteful and we may instead wish to track how many chunks we have and/or delete a chunk at random (or
	// just check e.g. 10 and delete the oldest of those) but we'll see if this is a bottleneck first. Paging
	// the data in is probably more expensive.
	TArray<APagedChunk*> toPageOut;
	uint32 chunkCount = 0;
	for (uint32 uIndex = 0; uIndex < CHUNK_ARRAY_SIZE; uIndex++)
	{
		if (ArrayChunks[uIndex])
		{
			chunkCount++;
			if (ArrayChunks[uIndex]->bDueToBePagedOut)
			{
				toPageOut.Add(ArrayChunks[uIndex]);
			}
		}
	}

	// Check if we have too many chunks, and delete the oldest if so.
	while (chunkCount > (uint32)ChunkCountLimit && toPageOut.Num() > 0)
	{
		toPageOut[0]->Destroy();
		toPageOut.RemoveAt(0);
	}
	return touchedChunks;
}

void UPagedVolumeComponent::FlushAll()
{
	// Clear this pointer as all chunks are about to be removed.
	LastAccessedChunk = NULL;

	// Erase all the most recently used chunks.
	for (uint32 uIndex = 0; uIndex < CHUNK_ARRAY_SIZE; uIndex++)
	{
		APagedChunk* chunk = ArrayChunks[uIndex];
		if (chunk != NULL)
		{
			chunk->RemoveChunk();
			chunk->Destroy();
		}
	}
	ArrayChunks.Empty();
}

int32 UPagedVolumeComponent::CalculateSizeInBytes() const
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
	return APagedChunk::CalculateSizeInBytes(ChunkSideLength) * uChunkCount;
}

void UPagedVolumeComponent::CreateMarchingCubesMesh(FRegion Region, TArray<FVoxelMaterial> VoxelMaterials)
{
	ChunkMaterials = VoxelMaterials;
	TArray<APagedChunk*> chunks = Prefetch(Region);
	for (int i = 0; i < chunks.Num(); i++)
	{
		// Queue the chunks; Tick will handle the actual chunk loading
		ChunksToCreateMesh.Enqueue(chunks[i]);
	}
}

uint8 UPagedVolumeComponent::GetChunkSideLength() const
{
	return ChunkSideLength;
}

uint8 UPagedVolumeComponent::GetSideLengthPower() const
{
	return ChunkSideLengthPower;
}

APagedChunk* UPagedVolumeComponent::GetLastAccessedChunk() const
{
	return LastAccessedChunk;
}

bool UPagedVolumeComponent::CanReuseLastAccessedChunk(int32 ChunkX, int32 ChunkY, int32 ChunkZ) const
{
	return ((ChunkX == LastAccessedChunkX) &&
		(ChunkY == LastAccessedChunkY) &&
		(ChunkZ == LastAccessedChunkZ) &&
		(LastAccessedChunk != NULL));
}

APagedChunk* UPagedVolumeComponent::GetChunk(int32 ChunkX, int32 ChunkY, int32 ChunkZ)
{
	APagedChunk* chunk = nullptr;

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
				chunk->bDueToBePagedOut = false;
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
		chunk = GetWorld()->SpawnActor<APagedChunk>();
		chunk->InitChunk(chunkPos, ChunkSideLength, Pager, VoxelSize, RandomSeed);
		chunk->bDueToBePagedOut = false;

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
		if (!bInsertedSucessfully)
		{
			UE_LOG(LogPolyVox, Fatal, TEXT("No space in chunk array for new chunk."));
			return NULL;
		}
	}

	LastAccessedChunk = chunk;
	LastAccessedChunkX = ChunkX;
	LastAccessedChunkY = ChunkY;
	LastAccessedChunkZ = ChunkZ;

	return chunk;
}


void UPagedVolumeComponent::FlattenRegionToHeight(const FRegion& Region, const int32 Height, FVoxel Filler)
{
	for (int x = Region.LowerX; x < Region.UpperX; x++)
	{
		for (int y = Region.LowerY; y < Region.UpperY; y++)
		{
			for (int z = Region.LowerZ; z < Region.UpperZ; z++)
			{
				FVoxel voxel = GetVoxelByCoordinates(x, y, z);
				if (z <= Height && !voxel.bIsSolid)
				{
					SetVoxelByCoordinates(x, y, z, Filler);
				}
				else if (z > Height && voxel.bIsSolid)
				{
					SetVoxelByCoordinates(x, y, z, FVoxel::GetEmptyVoxel());
				}
			}
		}
	}
}

void UPagedVolumeComponent::SetRegionHeightmap(const FRegion& Region, const TArray<float>& Heights, FVoxel Filler)
{
	TArray<float> resizedHeights;
	int32 regionWidth = URegionHelper::GetWidthInCells(Region);
	int32 targetSize = regionWidth * URegionHelper::GetHeightInCells(Region);
	if (Heights.Num() != targetSize)
	{
		// TODO Resize
	}

	for (int x = Region.LowerX; x < Region.UpperX; x++)
	{
		for (int y = Region.LowerY; y < Region.UpperY; y++)
		{
			float targetHeightPercent = UArrayHelper::Get2DFloat(Heights, x, y, regionWidth);
			int32 targetHeight = FMath::CeilToInt(URegionHelper::GetDepthInCells(Region) * targetHeightPercent);
			for (int z = Region.LowerZ; z < Region.UpperZ; z++)
			{
				if (z <= targetHeight)
				{
					SetVoxelByCoordinates(y, x, z, Filler);
				}
				else
				{
					SetVoxelByCoordinates(y, x, z, FVoxel::GetEmptyVoxel());
				}
			}
		}
	}
}

void UPagedVolumeComponent::SetRegionVoxels(const FRegion& Region, const TArray<float>& Heights, const TArray<uint8>& Materials)
{
	if (Heights.Num() != Materials.Num())
	{
		UE_LOG(LogPolyVox, Warning, TEXT("Height and Materials array size does not match. Heights: %d, Materials: %d"), Heights.Num(), Materials.Num());
	}

	int32 regionWidth = URegionHelper::GetWidthInCells(Region);
	int32 regionDepth = URegionHelper::GetDepthInCells(Region);

	for (int x = Region.LowerX; x < Region.UpperX; x++)
	{
		for (int y = Region.LowerY; y < Region.UpperY; y++)
		{
			uint8 targetMaterial = UArrayHelper::Get2DUint8(Materials, x, y, regionWidth);
			
			float targetHeightPercent = UArrayHelper::Get2DFloat(Heights, x, y, regionWidth);
			int32 targetHeight = Region.LowerZ + FMath::RoundToInt(regionDepth * targetHeightPercent);
			for (int z = Region.LowerZ; z < Region.UpperZ; z++)
			{
				if (z <= targetHeight)
				{
					SetVoxelByCoordinates(y, x, z, FVoxel::MakeVoxel(targetMaterial, true));
				}
				else
				{
					SetVoxelByCoordinates(y, x, z, FVoxel::GetEmptyVoxel());
				}
			}
		}
	}
}

void UPagedVolumeComponent::SetHeightmapFromImage(UTexture2D* Texture, FIntVector StartingPoint, int32 RegionHeight, FVoxel Filler)
{
	if (Texture == NULL)
	{
		UE_LOG(LogPolyVox, Error, TEXT("No texture defined!"));
		return;
	}

	uint32 textureWidth = Texture->GetSizeX();
	uint32 textureHeight = Texture->GetSizeY();

	TArray<float> floatArray;
	floatArray.SetNumZeroed(textureWidth * textureHeight);

	FTexture2DMipMap& Mip = Texture->PlatformData->Mips[0];//A reference 
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	uint8* raw = NULL;
	raw = (uint8*)Data;

	if (raw == NULL)
	{
		UE_LOG(LogPolyVox, Error, TEXT("Raw image data was null!"));
		Mip.BulkData.Unlock();
		Texture->UpdateResource();
		return;
	}

	FColor pixel = FColor(0, 0, 0, 255);//used for splitting the data stored in raw form
	for (uint32 y = 0; y < textureHeight; y++) {

		for (uint32 x = 0; x < textureWidth; x++) {
			// Data in the raw var is serialized
			// So a pixel is four consecutive numbers e.g 0,0,0,255
			// The following code splits the values in single components and stores them in a FColor
			pixel.B = raw[4 * (textureHeight * y + x) + 0];
			pixel.G = raw[4 * (textureHeight * y + x) + 1];
			pixel.R = raw[4 * (textureHeight * y + x) + 2];
			// And then this code iterates over the TArray of floats and stores them
			floatArray[x + y * textureHeight] = (((float)pixel.R / 255.0f) + ((float)pixel.G / 255.0f) + ((float)pixel.B / 255.0f)) / 3.0f;
		}
	}
	Mip.BulkData.Unlock();
	Texture->UpdateResource();

	FRegion region;
	region.LowerX = StartingPoint.X;
	region.LowerY = StartingPoint.Y;
	region.LowerZ = StartingPoint.Z;
	region.UpperX = region.LowerX + textureHeight;
	region.UpperY = region.LowerY + textureWidth;
	region.UpperZ = region.LowerZ + RegionHeight;

	SetRegionHeightmap(region, floatArray, Filler);
}

void UPagedVolumeComponent::SetRegionMaterials(const FRegion& Region, const TArray<uint8>& Materials, int32 BeginAtDepth, int32 PenetrateDistance)
{
	for (int x = Region.LowerX; x < Region.UpperX; x++)
	{
		for (int y = Region.LowerY; y < Region.UpperY; y++)
		{
			int32 currentVoxelDepth = -1;
			// We go "backwards" and start from the top of the region downward
			for (int z = Region.UpperZ - 1; z >= Region.LowerZ; z--)
			{
				FVoxel voxel = GetVoxelByCoordinates(x, y, z);
				if (voxel.bIsSolid)
				{
					currentVoxelDepth++;
					if (currentVoxelDepth >= BeginAtDepth && currentVoxelDepth < PenetrateDistance)
					{
						voxel.Material = UArrayHelper::Get2DUint8(Materials, x, y, URegionHelper::GetWidthInVoxels(Region));
						SetVoxelByCoordinates(x, y, z, voxel);
					}
					else if (currentVoxelDepth + BeginAtDepth >= PenetrateDistance)
					{
						break;
					}
				}
			}
		}

	}
}

void UPagedVolumeComponent::DrawVolumeAsDebug(const FRegion& DebugRegion)
{
	for (int x = DebugRegion.LowerX; x < DebugRegion.UpperX; x++)
	{
		for (int y = DebugRegion.LowerY; y < DebugRegion.UpperY; y++)
		{
			for (int z = DebugRegion.LowerZ; z < DebugRegion.UpperZ; z++)
			{
				FVoxel voxel = GetVoxelByCoordinates(x, y, z);
				if (!voxel.bIsSolid)
				{
					continue;
				}
				else
				{
					FVoxel neighbor = GetVoxelByCoordinates(x, y, z + 1);
					if (neighbor.bIsSolid)
					{
						continue;
					}
					FColor color = FColor::Red;

					FVector v0 = FVector(x * VoxelSize, y * VoxelSize, z * VoxelSize);
					FVector v1 = FVector(v0.X, v0.Y + VoxelSize, v0.Z);
					FVector v2 = FVector(v0.X + VoxelSize, v0.Y, v0.Z);
					FVector v3 = FVector(v2.X, v1.Y, v0.Z);
					//DrawDebugSphere(world, v0, 100, 4, color, true);
					DrawDebugLine(GetWorld(), v0, v1, color, true);
					DrawDebugLine(GetWorld(), v0, v2, color, true);
					DrawDebugLine(GetWorld(), v3, v2, color, true);
					DrawDebugLine(GetWorld(), v3, v1, color, true);
					//DrawDebugBox(GetWorld(), FVector(x * VoxelSize, y * VoxelSize, z * VoxelSize), FVector(VoxelSize, VoxelSize, VoxelSize), FColor::Red, true);
				}
			}
		}
	}
}