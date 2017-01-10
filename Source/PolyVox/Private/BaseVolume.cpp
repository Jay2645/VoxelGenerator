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
#include "ArrayHelper.h"
#include "DrawDebugHelpers.h"
#include "BaseVolume.h"


// Sets default values
ABaseVolume::ABaseVolume()
{
	PrimaryActorTick.bCanEverTick = false;
}

FVoxel ABaseVolume::GetVoxelByCoordinates(int32 xPos, int32 yPos, int32 zPos)
{
	unimplemented();
	return FVoxel();
}

FVoxel ABaseVolume::GetVoxelByVector(const FVector& Coordinates)
{
	unimplemented();
	return FVoxel();
}

void ABaseVolume::SetVoxelByCoordinates(int32 xPos, int32 yPos, int32 zPos, const FVoxel& Voxel)
{
	unimplemented();
}

void ABaseVolume::SetVoxelByVector(const FVector& Coordinates, const FVoxel& Voxel)
{
	unimplemented();
}

int32 ABaseVolume::CalculateSizeInBytes() const
{
	unimplemented();
	return 0;
}

uint8 ABaseVolume::GetSideLengthPower() const
{
	return 0;
}

void ABaseVolume::FlattenRegionToHeight(const FRegion& Region, const int32 Height, const FVoxel& Filler)
{
	for (int x = Region.LowerX; x < Region.UpperX; x++)
	{
		for (int y = Region.LowerY; y < Region.UpperY; y++)
		{
			for (int z = Region.LowerZ; z < Region.UpperZ; z++)
			{
				FVoxel voxel = GetVoxelByCoordinates(x, y, z);
				if (z <= Height && voxel.Density == 0)
				{
					SetVoxelByCoordinates(x, y, z, Filler);
				}
				else if (z > Height && voxel.Density != 0)
				{
					SetVoxelByCoordinates(x, y, z, FVoxel());
				}
			}
		}
	}
}

void ABaseVolume::SetRegionHeightmap(const FRegion& Region, const TArray<float>& Heights, const FVoxel& Filler)
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
			float targetHeightPercent = UArrayHelper::Get2DFloat(Heights,x, y, regionWidth);
			int32 targetHeight = FMath::CeilToInt(URegionHelper::GetDepthInCells(Region) * targetHeightPercent);
			for (int z = Region.LowerZ; z < Region.UpperZ; z++)
			{
				if (z <= targetHeight)
				{
					SetVoxelByCoordinates(x, y, z, Filler);
				}
				else
				{
					SetVoxelByCoordinates(x, y, z, FVoxel());
				}
			}
		}
	}
}

void ABaseVolume::SetHeightmapFromImage(UTexture2D* Texture, FIntVector StartingPoint, int32 RegionHeight, const FVoxel& Filler)
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

void ABaseVolume::SetRegionMaterials(const FRegion& Region, const TArray<uint8>& Materials, int32 BeginAtDepth, int32 PenetrateDistance)
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
				if (voxel.Density > 128)
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

void ABaseVolume::DrawVolumeAsDebug(const FRegion& DebugRegion)
{
	for (int x = DebugRegion.LowerX; x < DebugRegion.UpperX; x++)
	{
		for (int y = DebugRegion.LowerY; y < DebugRegion.UpperY; y++)
		{
			for (int z = DebugRegion.LowerZ; z < DebugRegion.UpperZ; z++)
			{
				FVoxel voxel = GetVoxelByCoordinates(x, y, z);
				if (voxel.Density == 0)
				{
					continue;
				}
				DrawDebugBox(GetWorld(), FVector(x * 100.0f, y * 100.0f, z * 100.0f), FVector(100.0f, 100.0f, 100.0f), FColor::Red, true);
			}
		}
	}
}
