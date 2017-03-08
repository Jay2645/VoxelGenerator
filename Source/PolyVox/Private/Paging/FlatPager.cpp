// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "FlatPager.h"

void UFlatPager::PageIn_Implementation(const FRegion& Region, APagedChunk* Chunk)
{
	for (int x = Region.LowerX; x < Region.UpperX; x++)
	{
		for (int y = Region.LowerY; y < Region.UpperY; y++)
		{
			for (int z = Region.LowerZ; z < Region.UpperZ; z++)
			{
				if (z > GroundLevel)
				{
					break;
				}
				FVoxel voxel = FVoxel::MakeVoxel(VoxelMaterial, true);
				Chunk->SetVoxelByCoordinatesWorldSpace(x, y, z, voxel);
			}
		}
	}
}
