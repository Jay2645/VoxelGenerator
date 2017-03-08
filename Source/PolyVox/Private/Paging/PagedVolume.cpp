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
	PagedVolumeComponent = CreateDefaultSubobject<UPagedVolumeComponent>(TEXT("Paged Volume"));
	/**
	* Total list of biomes:
	*		0 - OCEAN
	*		1 - ICE
	*		2 - MARSH
	*		3 - LAKE
	*		4 - COAST
	*		5 - SNOW
	*		6 - TUNDRA
	*		7 - BARE
	*		8 - SCORCHED
	*		9 - TAIGA
	*		10 - SHRUBLAND
	*		11 - TEMPERATE DESERT
	*		12 - TEMPERATE RAIN FOREST
	*		13 - TEMPERATE DECIDUOUS FOREST
	*		14 - GRASSLAND
	*		15 - TROPICAL RAIN FOREST
	*		16 - TROPICAL SEASONAL FOREST
	*		17 - SUBTROPICAL DESERT
	*/
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
	VolumeLandMaterials.Add(FVoxelMaterial());
}

UPagedVolumeComponent* APagedVolume::GetPagedVolume() const
{
	return PagedVolumeComponent;
}
