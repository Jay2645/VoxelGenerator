/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2017 Jay Stevens

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

#include "Engine/DataAsset.h"
#include "Voxel.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct POLYVOX_API FVoxel
{
	GENERATED_BODY()
public:	
	// An index referring to this voxel's "Material."
	// This can be either a UENUM or a hardcoded uint8 value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	uint8 Material;
	// Whether this voxel is solid or air.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	bool bIsSolid;

	FVoxel()
	{
		Material = 0;
		bIsSolid = false;
	}

	static FVoxel GetEmptyVoxel();

	static FVoxel MakeVoxel(uint8 MaterialID, bool bShouldBeSolid);
};
