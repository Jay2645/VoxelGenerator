// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "MarchingCubesDefaultController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POLYVOX_API UMarchingCubesDefaultController : public UObject
{
	GENERATED_BODY()
public:

	uint8 ConvertToDensity(const FVoxel& Voxel);

	/**
	* Returns a material which is in some sense a weighted combination of the supplied materials.
	*
	* The Marching Cubes algorithm generates vertices which lie between voxels, and ideally the material of the vertex should be interpolated from the materials
	* of the voxels. In practice, that material type is often an integer identifier (e.g. 1 = rock, 2 = soil, 3 = grass) and an interpolation doesn't make sense
	* (e.g. soil is not a combination or rock and grass). Therefore this default interpolation just returns whichever material is associated with a voxel of the
	* higher density, but if more advanced voxel types do support interpolation then it can be implemented in this function.
	*/
	FVoxel BlendMaterials(const FVoxel& FirstVoxel, const FVoxel& SecondVoxel, const float Interpolation);
	uint8 GetThreshold();

protected:
	uint8 Threshold = 127;
};
