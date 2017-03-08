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
#include "MarchingCubesDefaultController.h"


uint8 UMarchingCubesDefaultController::ConvertToDensity(const FVoxel& Voxel, const FGameplayTag& Prefix) const
{
	if (Voxel.VoxelType.IsValid() && Voxel.VoxelType.MatchesTag(Prefix))
	{
		return 255;
	}
	else
	{
		return 0;
	}
}

FVoxel UMarchingCubesDefaultController::BlendMaterials(FVoxel FirstVoxel, FVoxel SecondVoxel, const FGameplayTag& Prefix, const float Interpolation) const
{
	if (ConvertToDensity(FirstVoxel, Prefix) > ConvertToDensity(SecondVoxel, Prefix))
	{
		return FirstVoxel;
	}
	else
	{
		return SecondVoxel;
	}
}

uint8 UMarchingCubesDefaultController::GetThreshold() const
{
	return Threshold;
}
