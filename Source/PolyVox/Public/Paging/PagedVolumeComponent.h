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

#pragma once

#include "Components/ActorComponent.h"
#include "Pager.h"
#include "Containers/Queue.h"
#include "PagedVolumeComponent.generated.h"

class APagedChunk;
struct FVoxelMaterial;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYVOX_API UPagedVolumeComponent : public UActorComponent
{
	friend class APagedVolume;
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPagedVolumeComponent();
	~UPagedVolumeComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pager")
	TSubclassOf<UPager> VolumePager;
	UPROPERTY()
	int32 TargetMemoryUsageInBytes = 268435456;
	// The size of the chunks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	uint8 ChunkSideLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxels")
	float VoxelSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	int32 RandomSeed = 123;

	UFUNCTION(BlueprintPure, Category = "Volume|Voxels")
		virtual FVoxel GetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos);
	UFUNCTION(BlueprintPure, Category = "Volume|Voxels")
		virtual FVoxel GetVoxelByVector(const FVector& Coordinates);

	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
		virtual void SetVoxelByCoordinates(int32 XPos, int32 YPos, int32 ZPos, FVoxel Voxel);
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
		virtual void SetVoxelByVector(const FVector& Coordinates, FVoxel Voxel);

	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
	virtual void PageInChunksAroundPlayer(AController* PlayerController, const int32& MaxWorldHeight, const uint8& NumberOfChunksToPageIn, TArray<FVoxelMaterial> Materials, bool bUseMarchingCubes);

	// Tries to ensure that the voxels within the specified Region are loaded into memory.
	UFUNCTION(BlueprintCallable, Category = "Volume|Utility")
		TArray<APagedChunk*> Prefetch(FRegion PrefetchRegion);
	// Removes all voxels from memory
	UFUNCTION(BlueprintCallable, Category = "Volume|Utility")
		void FlushAll();

	//UFUNCTION(BlueprintPure, Category = "Volume|Utility")
	//virtual bool RegionIsEmpty(const FRegion& Region) override;

	UFUNCTION(BlueprintPure, Category = "Volume|Utility")
		virtual int32 CalculateSizeInBytes() const;

	UFUNCTION(BlueprintCallable, Category = "Volume|Mesh")
		void CreateMarchingCubesMesh(FRegion Region, TArray<FVoxelMaterial> VoxelMaterials);

	virtual uint8 GetChunkSideLength() const;
	virtual uint8 GetSideLengthPower() const;
	APagedChunk* GetLastAccessedChunk() const;
	bool CanReuseLastAccessedChunk(int32 iChunkX, int32 iChunkY, int32 iChunkZ) const;
	APagedChunk* GetChunk(int32 uChunkX, int32 uChunkY, int32 uChunkZ);

	// Flattens a region to be exactly a specific height.
	// Any Voxels above this height are turned to air.
	// Voxels below this height are brought up to be of the specified height, using a given Voxel as a "filler".
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
		void FlattenRegionToHeight(const FRegion& Region, const int32 Height, FVoxel Filler);

	// This changes a region to reflect a given heightmap.
	// This heightmap should be a 2D array, with dimensions the same as the width (x) and height (y) of the region being passed into it.
	// The Voxel passed into this function is used for any solid voxels in the heightmap.
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
		void SetRegionHeightmap(const FRegion& Region, const TArray<float>& Heights, FVoxel Filler);

	// This changes a region to reflect a given heightmap.
	// This heightmap should be a 2D array, with dimensions the same as the width (x) and height (y) of the region being passed into it.
	// The Voxel passed into this function is used for any solid voxels in the heightmap.
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
		void SetRegionVoxels(const FRegion& Region, const TArray<float>& Heights, const TArray<uint8>& Materials);

	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
		void SetHeightmapFromImage(UTexture2D* Texture, FIntVector StartingPoint, int32 RegionHeight, FVoxel Filler);

	// This goes through an entire region and maps any solid voxels to the material ID for that point in the Materials array.
	// This heightmap should be a 2D array, with dimensions the same as the width (x) and height (y) of the region being passed into it.
	// Begin at Depth specifies how far beneath the surface the painting will start.
	// Penetrate Distance is how many voxels deep the material will extend below the surface.
	UFUNCTION(BlueprintCallable, Category = "Volume|Voxels")
		void SetRegionMaterials(const FRegion& Region, const TArray<uint8>& Materials, int32 BeginAtDepth = 0, int32 PenetrateDistance = 256);

	UFUNCTION(BlueprintCallable, Category = "Volume|Debug")
		void DrawVolumeAsDebug(const FRegion& DebugRegion);

	UPROPERTY()
		APagedChunk* LastAccessedChunk = nullptr;

private:
	TQueue<APagedChunk*> ChunksToCreateMesh;
	UPROPERTY()
		APagedChunk* ChunkCurrentlyMakingMeshFor;
	UPROPERTY()
		TArray<FVoxelMaterial> ChunkMaterials;

	UPROPERTY()
		int32 LastAccessedChunkX = 0;
	UPROPERTY()
		int32 LastAccessedChunkY = 0;
	UPROPERTY()
		int32 LastAccessedChunkZ = 0;

	UPROPERTY()
		int32 ChunkCountLimit = 0;

	// Chunks are stored in the following array which is used as a hash-table. Conventional wisdom is that such a hash-table
	// should not be more than half full to avoid conflicts, and a practical chunk size seems to be 64^3. With this configuration
	// there can be up to 32768*64^3 = 8 gigavoxels (with each voxel perhaps being many bytes). This should effectively make use 
	// of even high end machines. Of course, the user can choose to limit the memory usage in which case much less of the chunk 
	// array will actually be used. None-the-less, we have chosen to use a fixed size array (rather than a vector) as it appears to 
	// be slightly faster (probably due to the extra pointer indirection in a vector?) and the actual size of this array should
	// just be 1Mb or so.
	static const uint32 CHUNK_ARRAY_SIZE = 65536;
	UPROPERTY()
		TArray<APagedChunk*> ArrayChunks;

	UPROPERTY()
		uint8 ChunkSideLengthPower;
	UPROPERTY()
		int32 ChunkMask;

	UPROPERTY()
		UPager* Pager = nullptr;

	UFUNCTION()
		// 256 * 1024 * 1024 = 268435456
		void InitializeVolume(TSubclassOf<UPager> PagerClass, int32 MemoryUsageInBytes = 268435456, uint8 VolumeChunkSideLength = 32);
};
