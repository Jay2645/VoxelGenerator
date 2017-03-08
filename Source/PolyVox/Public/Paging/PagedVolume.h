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

#include "PagedVolumeComponent.h"
#include "Pager.h"
#include "PagedChunk.h"
#include "Mesh/VoxelProceduralMeshComponent.h"
#include "PagedVolume.generated.h"

/**
 * 
 */
 UCLASS(Blueprintable)
class POLYVOX_API APagedVolume : public AActor
{
	GENERATED_BODY()
public:
	APagedVolume();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume")
	UPagedVolumeComponent* PagedVolumeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxels")
	TArray<FVoxelMaterial> VolumeLandMaterials;

	UFUNCTION(BlueprintCallable, Category = "Volume")
	UPagedVolumeComponent* GetPagedVolume() const;
};
