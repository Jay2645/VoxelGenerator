// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "VoxelHelper.h"
#include "Region.h"

void URegion::CreateRegion(FVector StartingLocation, FVector EndingLocation)
{
	ChunkRegion = PolyVox::Region(PolyVox::Vector3DInt32((int32)StartingLocation.X, (int32)StartingLocation.Y, (int32)StartingLocation.Z), PolyVox::Vector3DInt32((int32)EndingLocation.X, (int32)EndingLocation.Y, (int32)EndingLocation.Z));
}

void URegion::CreateRegionFromPolyVox(PolyVox::Region OurRegion)
{
	ChunkRegion = OurRegion;
}

PolyVox::Region URegion::GetRegion() const
{
	return ChunkRegion;
}

FVector URegion::GetCenter() const
{
	return FPolyVoxVector(ChunkRegion.getCentre());
}

FVector URegion::GetLowerCorner() const
{
	return FPolyVoxVector(ChunkRegion.getLowerCorner());
}

FVector URegion::GetUpperCorner() const
{
	return FPolyVoxVector(ChunkRegion.getUpperCorner());
}

int32 URegion::GetCenterX() const
{
	return (int32)ChunkRegion.getCentreX();
}

int32 URegion::GetCenterY() const
{
	return (int32)ChunkRegion.getCentreY();
}

int32 URegion::GetCenterZ() const
{
	return (int32)ChunkRegion.getCentreZ();
}

int32 URegion::GetLowerX() const
{
	return (int32)ChunkRegion.getLowerX();
}

int32 URegion::GetLowerY() const
{
	return (int32)ChunkRegion.getLowerY();
}

int32 URegion::GetLowerZ() const
{
	return (int32)ChunkRegion.getLowerZ();
}

int32 URegion::GetUpperX() const
{
	return (int32)ChunkRegion.getUpperX();
}

int32 URegion::GetUpperY() const
{
	return (int32)ChunkRegion.getUpperY();
}

int32 URegion::GetUpperZ() const
{
	return (int32)ChunkRegion.getUpperZ();
}

int32 URegion::GetWidthInVoxels() const
{
	return (int32)ChunkRegion.getWidthInVoxels();
}

int32 URegion::GetHeightInVoxels() const
{
	return (int32)ChunkRegion.getHeightInVoxels();
}

int32 URegion::GetDepthInVoxels() const
{
	return (int32)ChunkRegion.getDepthInVoxels();
}

FVector URegion::GetDimensionsInVoxels() const
{
	return FPolyVoxVector(ChunkRegion.getDimensionsInVoxels());
}
