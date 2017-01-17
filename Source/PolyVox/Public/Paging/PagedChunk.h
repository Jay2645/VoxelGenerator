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

#include "UObject/NoExportTypes.h"
#include "RegionHelper.h"
#include "Pager.h"
#include "Mesh/VoxelProceduralMeshComponent.h"
#include "PagedChunk.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POLYVOX_API APagedChunk : public AActor
{
	friend class APagedVolume;
	GENERATED_BODY()
public:
	APagedChunk();
	~APagedChunk();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UVoxelProceduralMeshComponent* VoxelMesh;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Voxels")
	FRegion ChunkRegion;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Voxels")
	bool bDueToBePagedOut;

	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void InitChunk(FVector Position, uint8 ChunkSideLength, UPager* VoxelPager = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void RemoveChunk();

	UFUNCTION(BlueprintPure, Category="Chunk|Voxels")
	TArray<FVoxel> GetData() const;
	UFUNCTION(BlueprintPure, Category = "Chunk|Size")
	int32 GetDataSizeInBytes() const;

	UFUNCTION(BlueprintPure, Category = "Chunk|Voxels")
	FVoxel GetVoxelByCoordinatesWorldSpace(int32 XPos, int32 YPos, int32 ZPos);
	UFUNCTION(BlueprintPure, Category = "Chunk|Voxels")
	FVoxel GetVoxelByCoordinatesChunkSpace(int32 XPos, int32 YPos, int32 ZPos);

	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void SetVoxelByCoordinatesWorldSpace(int32 XPos, int32 YPos, int32 ZPos, FVoxel Value);
	UFUNCTION(BlueprintCallable, Category = "Chunk|Voxels")
	void SetVoxelByCoordinatesChunkSpace(int32 XPos, int32 YPos, int32 ZPos, FVoxel Value);

	UFUNCTION(BlueprintCallable, Category = "Volume|Mesh")
	void CreateMarchingCubesMesh(ABaseVolume* Volume, TArray<FVoxelMaterial> VoxelMaterials);
	
	FVoxel GetDataAtIndex(const int32 CurrentVoxelIndex) const;

private:
	static int32 CalculateSizeInBytes(uint8 ChunkSideLength);

	// This is so we can tell whether a uncompressed chunk has to be recompressed and whether
	// a compressed chunk has to be paged back to disk, or whether they can just be discarded.
	UPROPERTY()
	bool bDataModified;
	UPROPERTY()
	bool bNeedsNewMarchingCubesMesh;

	UPROPERTY()
	TArray<FVoxel> VoxelData;
	UPROPERTY()
	uint8 SideLength;
	UPROPERTY()
	uint8 SideLengthPower;
	UPROPERTY()
	UPager* Pager;

	UPROPERTY()
	FVector ChunkSpacePosition;
};
