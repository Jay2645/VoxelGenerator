// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RegionHelper.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPolyVox, Log, All);

USTRUCT(BlueprintType)
struct POLYVOX_API FVoxel
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Voxel")
	uint8 Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	uint8 Density;

	FVoxel()
	{
		Material = 0;
		Density = 0;
	}
};

/** Represents a part of a Volume.
*
*  Many operations in PolyVox are constrained to only part of a volume. For example, when running the surface extractors
*  it is unlikely that you will want to run it on the whole volume at once, as this will give a very large mesh which may
*  be too much to render. Instead you will probably want to run a surface extractor a number of times on different parts
*  of the volume, there by giving a number of meshes which can be culled and rendered separately.
*
*  The Region class is used to define these parts (regions) of the volume. Essentially it consists of an upper and lower
*  bound which specify the range of voxels positions considered to be part of the region. Note that these bounds are
*  <em>inclusive</em>.
*
*  Utility functions can be found in URegionHelper.
*/
USTRUCT(BlueprintType)
struct POLYVOX_API FRegion
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	int32 LowerX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	int32 LowerY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	int32 LowerZ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	int32 UpperX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	int32 UpperY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	int32 UpperZ;

	FRegion()
	{
		LowerX = 0;
		LowerY = 0;
		LowerZ = 0;

		UpperX = 0;
		UpperY = 0;
		UpperZ = 0;
	}
	
	FORCEINLINE bool operator==(const FRegion& rhs) const
	{
		return	((LowerX == rhs.LowerX) && (LowerY == rhs.LowerY) && (LowerZ == rhs.LowerZ)
				&& (UpperX == rhs.UpperX) && (UpperY == rhs.UpperY) && (UpperZ == rhs.UpperZ));
	}
	FORCEINLINE bool operator!=(const FRegion& rhs) const
	{
		return !(*this == rhs);
	}
};

/** Represents a part of a Volume.
*  This class also provide utility functions for increasing and decreasing the size of the Region, shifting the 
*  Region in 3D space, testing whether it contains a given position, enlarging it so that
*  it does contain a given position, cropping it to another Region, and various other utility functions.
*/
UCLASS()
class POLYVOX_API URegionHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// A Region with the lower corner set as low as possible and the upper corner set as high as possible.
	UFUNCTION(BlueprintPure, Category = "Region|Constants")
	static FRegion GetMaxRegion();
	/// A Region with the lower corner set as high as possible and the upper corner set as low as possible.
	UFUNCTION(BlueprintPure, Category = "Region|Constants")
	static FRegion GetInvertedRegion();
	
	/// A Region with the given lower and upper corners.
	UFUNCTION(BlueprintPure, Category = "Region|Constructors")
	static FRegion CreateRegionFromVector(const FVector& LowerCorner, const FVector& UpperCorner);
	/// A Region using the given ints for its upper and lower corners.
	UFUNCTION(BlueprintPure, Category = "Region|Constructors")
	static FRegion CreateRegionFromInt(int32 LowerX, int32 LowerY, int32 LowerZ, int32 UpperX, int32 UpperY, int32 UpperZ);

	/// Gets the 'x' position of the center.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetCenterX(const FRegion& Region);
	/// Gets the 'y' position of the center.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetCenterY(const FRegion& Region);
	/// Gets the 'z' position of the center.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetCenterZ(const FRegion& Region);
	/// Gets the 'x' position of the lower corner
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetLowerX(const FRegion& Region);
	/// Gets the 'y' position of the lower corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetLowerY(const FRegion& Region);
	/// Gets the 'z' position of the lower corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetLowerZ(const FRegion& Region);
	/// Gets the 'x' position of the upper corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetUpperX(const FRegion& Region);
	/// Gets the 'y' position of the upper corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetUpperY(const FRegion& Region);
	/// Gets the 'z' position of the upper corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetUpperZ(const FRegion& Region);

	/// Gets the center of the region
	UFUNCTION(BlueprintPure, Category = "Region")
	static FVector GetCenter(const FRegion& Region);
	/// Gets the position of the lower corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	static FVector GetLowerCorner(const FRegion& Region);
	/// Gets the position of the upper corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	static FVector GetUpperCorner(const FRegion& Region);

	/// Gets the width of the region measured in voxels.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetWidthInVoxels(const FRegion& Region);
	/// Gets the height of the region measured in voxels.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetHeightInVoxels(const FRegion& Region);
	/// Gets the depth of the region measured in voxels.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetDepthInVoxels(const FRegion& Region);
	/// Gets the dimensions of the region measured in voxels.
	UFUNCTION(BlueprintPure, Category = "Region")
	static FVector GetDimensionsInVoxels(const FRegion& Region);

	/// Gets the width of the region measured in cells.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetWidthInCells(const FRegion& Region);
	/// Gets the height of the region measured in cells.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetHeightInCells(const FRegion& Region);
	/// Gets the depth of the region measured in cells.
	UFUNCTION(BlueprintPure, Category = "Region")
	static int32 GetDepthInCells(const FRegion& Region);
	/// Gets the dimensions of the region measured in cells.
	UFUNCTION(BlueprintPure, Category = "Region")
	static FVector GetDimensionsInCells(const FRegion& Region);

	/// Sets the position of the lower corner.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion SetLowerCorner(FRegion& Region, const FVector& LowerCorner);
	/// Sets the position of the upper corner.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion SetUpperCorner(FRegion& Region, const FVector& UpperCorner);

	/// Tests whether the given point is contained in this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsFloatPoint(const FRegion& Region, float fX, float fY, float fZ, float boundary = 0.0f);
	/// Tests whether the given point is contained in this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsVectorPoint(const FRegion& Region, const FVector& pos, float boundary = 0.0f);
	/// Tests whether the given point is contained in this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsIntPoint(const FRegion& Region, int32 iX, int32 iY, int32 iZ, uint8 boundary = 0);
	/// Tests whether the given position is contained in the 'x' range of this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsFloatPointInX(const FRegion& Region, float pos, float boundary = 0.0f);
	/// Tests whether the given position is contained in the 'x' range of this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsIntPointInX(const FRegion& Region, int32 pos, uint8 boundary = 0);
	/// Tests whether the given position is contained in the 'y' range of this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsFloatPointInY(const FRegion& Region, float pos, float boundary = 0.0f);
	/// Tests whether the given position is contained in the 'y' range of this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsIntPointInY(const FRegion& Region, int32 pos, uint8 boundary = 0);
	/// Tests whether the given position is contained in the 'z' range of this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsFloatPointInZ(const FRegion& Region, float pos, float boundary = 0.0f);
	/// Tests whether the given position is contained in the 'z' range of this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsIntPointInZ(const FRegion& Region, int32 pos, uint8 boundary = 0);

	/// Tests whether the given Region is contained in this Region.
	UFUNCTION(BlueprintPure, Category = "Region|Contains")
	static bool ContainsRegion(const FRegion& Region, const FRegion& other, uint8 boundary = 0);

	/// Enlarges the Region so that it contains the specified position.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion AccumulateInt(FRegion& Region, int32 iX, int32 iY, int32 iZ);
	/// Enlarges the Region so that it contains the specified position.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion AccumulateVector(FRegion& Region, const FVector& v3dPos);
	/// Enlarges the Region so that it contains the specified Region.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion AccumulateRegion(FRegion& Region, const FRegion& other);

	/// Crops the extents of this Region according to another Region.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion CropTo(FRegion& Region, const FRegion& other);

	/// Grows this region by the amount specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion GrowConstantAmount(FRegion& Region, int32 Amount);
	/// Grows this region by the amounts specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion GrowByAxis(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ);
	/// Grows this region by the amounts specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion GrowVector(FRegion& Region, const FVector&  Amount);

	/// Tests whether all components of the upper corner are at least
	/// as great as the corresponding components of the lower corner.
	UFUNCTION(BlueprintPure, Category = "Region")
	static bool IsValid(const FRegion& Region);

	/// Moves the Region by the amount specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion ShiftInt(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ);
	/// Moves the Region by the amount specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion ShiftVector(FRegion& Region, const FVector& Amount);
	/// Moves the lower corner of the Region by the amount specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion ShiftIntLowerCorner(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ);
	/// Moves the lower corner of the Region by the amount specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion ShiftVectorLowerCorner(FRegion& Region, const FVector& Amount);
	/// Moves the upper corner of the Region by the amount specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion ShiftIntUpperCorner(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ);
	/// Moves the upper corner of the Region by the amount specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion ShiftVectorUpperCorner(FRegion& Region, const FVector& Amount);

	/// Shrinks this region by the amount specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion ShrinkConstantAmount(FRegion& Region, int32 Amount);
	/// Shrinks this region by the amounts specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion ShrinkByAxis(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ);
	/// Shrinks this region by the amounts specified.
	UFUNCTION(BlueprintCallable, Category = "Region|Modify")
	static FRegion ShrinkVector(FRegion& Region, const FVector& Amount);

	UFUNCTION(BlueprintPure, Category = "Region")
	static bool Intersects(const FRegion& Region, const FRegion& Other);
};