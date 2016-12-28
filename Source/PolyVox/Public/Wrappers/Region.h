// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "PolyVox/PagedVolume.h"
#include "Region.generated.h"

/** Represents a part of a Volume.
*
*  Many operations in PolyVox are constrained to only part of a volume. For example, when running the surface extractors
*  it is unlikely that you will want to run it on the whole volume at once, as this will give a very large mesh which may
*  be too much to render. Instead you will probably want to run a surface extractor a number of times on different parts
*  of the volume, there by giving a number of meshes which can be culled and rendered separately.
*
*  The Region class is used to define these parts (regions) of the volume. Essentially it consists of an upper and lower
*  bound which specify the range of voxel positions considered to be part of the region. Note that these bounds are
*  <em>inclusive</em>.
*
*  As well as the expected set of getters and setters, this class also provide utility functions for increasing and decreasing
*  the size of the Region, shifting the Region in 3D space, testing whether it contains a given position, enlarging it so that
*  it does contain a given position, cropping it to another Region, and various other utility functions.
*
*/
UCLASS(BlueprintType)
class POLYVOX_API URegion : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Region")
	void CreateRegion(FVector StartingLocation, FVector EndingLocation);

	void CreateRegionFromPolyVox(PolyVox::Region OurRegion);

	PolyVox::Region GetRegion() const;
private:
	PolyVox::Region ChunkRegion;

public:
	// Gets the center of the region
	UFUNCTION(BlueprintPure, Category = "Region")
	FVector GetCenter() const;
	// Gets the position of the lower corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	FVector GetLowerCorner() const;
	// Gets the position of the upper corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	FVector GetUpperCorner() const;

	// Gets the 'x' position of the center.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetCenterX() const;
	// Gets the 'y' position of the center.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetCenterY() const;
	// Gets the 'z' position of the center.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetCenterZ() const;
	// Gets the 'x' position of the lower corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetLowerX() const;
	// Gets the 'y' position of the lower corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetLowerY() const;
	// Gets the 'z' position of the lower corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetLowerZ() const;
	// Gets the 'x' position of the upper corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetUpperX() const;
	// Gets the 'y' position of the upper corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetUpperY() const;
	// Gets the 'z' position of the upper corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetUpperZ() const;

	// Gets the width of the region measured in voxels.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetWidthInVoxels() const;
	// Gets the height of the region measured in voxels.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetHeightInVoxels() const;
	// Gets the depth of the region measured in voxels.
	UFUNCTION(BlueprintPure, Category = "Region")
	int32 GetDepthInVoxels() const;
	// Gets the dimensions of the region measured in voxels.
	UFUNCTION(BlueprintPure, Category = "Region")
	FVector GetDimensionsInVoxels() const;
};
