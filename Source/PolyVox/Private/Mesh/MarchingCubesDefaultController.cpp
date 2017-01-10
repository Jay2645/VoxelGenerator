// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "MarchingCubesDefaultController.h"


uint8 UMarchingCubesDefaultController::ConvertToDensity(const FVoxel& Voxel)
{
	return Voxel.Density;
}

FVoxel UMarchingCubesDefaultController::BlendMaterials(const FVoxel& FirstVoxel, const FVoxel& SecondVoxel, const float Interpolation)
{
	if (ConvertToDensity(FirstVoxel) > ConvertToDensity(SecondVoxel))
	{
		return FirstVoxel;
	}
	else
	{
		return SecondVoxel;
	}
}

uint8 UMarchingCubesDefaultController::GetThreshold()
{
	return Threshold;
}
