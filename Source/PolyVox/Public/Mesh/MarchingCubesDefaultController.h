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

#include "Voxel.h"
#include "MarchingCubesDefaultController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POLYVOX_API UMarchingCubesDefaultController : public UObject
{
	GENERATED_BODY()
public:

	uint8 ConvertToDensity(FVoxel Voxel);

	/**
	* Returns a material which is in some sense a weighted combination of the supplied materials.
	*
	* The Marching Cubes algorithm generates vertices which lie between voxels, and ideally the material of the vertex should be interpolated from the materials
	* of the voxels. In practice, that material type is often an integer identifier (e.g. 1 = rock, 2 = soil, 3 = grass) and an interpolation doesn't make sense
	* (e.g. soil is not a combination or rock and grass). Therefore this default interpolation just returns whichever material is associated with a voxel of the
	* higher density, but if more advanced voxel types do support interpolation then it can be implemented in this function.
	*/
	FVoxel BlendMaterials(FVoxel FirstVoxel, FVoxel SecondVoxel, const float Interpolation);
	uint8 GetThreshold();

protected:
	uint8 Threshold = 127;
};
