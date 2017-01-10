// Fill out your copyright notice in the Description page of Project Settings.

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
		// Resize
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

void ABaseVolume::SetRegionHeightmapFromImage(const FRegion& Region, UTexture2D* Texture, const FVoxel& Filler)
{
	if (Texture == NULL)
	{
		UE_LOG(LogPolyVox, Error, TEXT("No texture defined!"));
	}
	FTexture2DMipMap* mipMap = &Texture->PlatformData->Mips[0];
	FByteBulkData* rawData = &mipMap->BulkData;
	FColor* formatedImageData = static_cast<FColor*>(rawData->Lock(LOCK_READ_ONLY));
	
	TArray<float> floatArray;

	uint32 textureWidth = mipMap->SizeX;
	uint32 textureHeight = mipMap->SizeY;
	for (uint32 x = 0; x < textureWidth; x++)
	{
		for (uint32 y = 0; y < textureHeight; y++)
		{
			FColor color = formatedImageData[y * textureWidth + x];
			float redAmount = color.R / 255;
			float greenAmount = color.G / 255;
			float blueAmount = color.B / 255;

			float gray = (redAmount + greenAmount + blueAmount) / 3.0f;
			floatArray.Add(gray); 
		}
	}

	Texture->PlatformData->Mips[0].BulkData.Unlock();

	SetRegionHeightmap(Region, floatArray, Filler);
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
