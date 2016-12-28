// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "Chunk.h"

void UChunk::SetChunk(PolyVox::PagedVolume<PolyVox::MaterialDensityPair44>::Chunk* PolyVoxChunk)
{
	PagedChunk = MakeShareable(PolyVoxChunk);
}

void UChunk::SetVoxel(int32 XCoord, int32 YCoord, int32 ZCoord, int32 Material, int32 Density)
{
	PolyVox::MaterialDensityPair44 voxel;
	voxel.setMaterial(Material);
	voxel.setDensity(Density);
	SetVoxelFromPolyVox(XCoord, YCoord, ZCoord, voxel);
}

void UChunk::SetVoxelFromPolyVox(int32 XCoord, int32 YCoord, int32 ZCoord, PolyVox::MaterialDensityPair44 Voxel)
{
	PagedChunk->setVoxel(XCoord, YCoord, ZCoord, Voxel);
}

int32 UChunk::GetVoxelMaterial(int32 XCoord, int32 YCoord, int32 ZCoord) const
{
	return PagedChunk->getVoxel(XCoord, YCoord, ZCoord).getMaterial();
}

int32 UChunk::GetVoxelDensity(int32 XCoord, int32 YCoord, int32 ZCoord) const
{
	return PagedChunk->getVoxel(XCoord, YCoord, ZCoord).getDensity();
}