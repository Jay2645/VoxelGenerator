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
#include "RegionHelper.h"

DEFINE_LOG_CATEGORY(LogPolyVox);

FRegion URegionHelper::GetMaxRegion()
{
	FRegion region;
	region.LowerX = INT_MIN;
	region.LowerY = INT_MIN;
	region.LowerZ = INT_MIN;
	region.UpperX = INT_MAX;
	region.UpperY = INT_MAX;
	region.UpperZ = INT_MAX;
	return region;
}

FRegion URegionHelper::GetInvertedRegion()
{
	FRegion region;
	region.UpperX = INT_MIN;
	region.UpperY = INT_MIN;
	region.UpperZ = INT_MIN;
	region.LowerX = INT_MAX;
	region.LowerY = INT_MAX;
	region.LowerZ = INT_MAX;
	return region;
}

FRegion URegionHelper::CreateRegionFromVector(const FVector& LowerCorner, const FVector& UpperCorner)
{
	FRegion region;
	region.LowerX = FMath::RoundToInt(LowerCorner.X);
	region.LowerY = FMath::RoundToInt(LowerCorner.Y);
	region.LowerZ = FMath::RoundToInt(LowerCorner.Z);
	region.UpperX = FMath::RoundToInt(UpperCorner.X);
	region.UpperY = FMath::RoundToInt(UpperCorner.Y);
	region.UpperZ = FMath::RoundToInt(UpperCorner.Z);
	return region;
}

FRegion URegionHelper::CreateRegionFromInt(int32 LowerX, int32 LowerY, int32 LowerZ, int32 UpperX, int32 UpperY, int32 UpperZ)
{
	FRegion region;
	region.LowerX = LowerX;
	region.LowerY = LowerY;
	region.LowerZ = LowerZ;
	region.UpperX = UpperX;
	region.UpperY = UpperY;
	region.UpperZ = UpperZ;
	return region;
}

int32 URegionHelper::GetCenterX(const FRegion& Region)
{
	return (Region.LowerX + Region.UpperX) / 2;
}

int32 URegionHelper::GetCenterY(const FRegion& Region)
{
	return (Region.LowerY + Region.UpperY) / 2;
}

int32 URegionHelper::GetCenterZ(const FRegion& Region)
{
	return (Region.LowerZ + Region.UpperZ) / 2;
}

int32 URegionHelper::GetLowerX(const FRegion& Region)
{
	return Region.LowerX;
}

int32 URegionHelper::GetLowerY(const FRegion& Region)
{
	return Region.LowerY;
}

int32 URegionHelper::GetLowerZ(const FRegion& Region)
{
	return Region.LowerZ;
}

int32 URegionHelper::GetUpperX(const FRegion& Region)
{
	return Region.UpperX;
}

int32 URegionHelper::GetUpperY(const FRegion& Region)
{
	return Region.UpperY;
}

int32 URegionHelper::GetUpperZ(const FRegion& Region)
{
	return Region.UpperZ;
}

FVector URegionHelper::GetCenter(const FRegion& Region)
{
	return FVector(GetCenterX(Region), GetCenterY(Region), GetCenterZ(Region));
}

FVector URegionHelper::GetLowerCorner(const FRegion& Region)
{
	return FVector(GetLowerX(Region), GetLowerY(Region), GetLowerZ(Region));
}

FVector URegionHelper::GetUpperCorner(const FRegion& Region)
{
	return FVector(GetUpperX(Region), GetUpperY(Region), GetUpperZ(Region));
}

int32 URegionHelper::GetWidthInVoxels(const FRegion& Region)
{
	return GetWidthInCells(Region) + 1;
}

int32 URegionHelper::GetHeightInVoxels(const FRegion& Region)
{
	return GetHeightInCells(Region) + 1;
}

int32 URegionHelper::GetDepthInVoxels(const FRegion& Region)
{
	return GetDepthInCells(Region) + 1;
}

FVector URegionHelper::GetDimensionsInVoxels(const FRegion& Region)
{
	return GetDimensionsInCells(Region) + FVector(1, 1, 1);
}

int32 URegionHelper::GetWidthInCells(const FRegion& Region)
{
	return Region.UpperX - Region.LowerX;
}

int32 URegionHelper::GetHeightInCells(const FRegion& Region)
{
	return Region.UpperY - Region.LowerY;
}

int32 URegionHelper::GetDepthInCells(const FRegion& Region)
{
	return Region.UpperZ - Region.LowerZ;
}

FVector URegionHelper::GetDimensionsInCells(const FRegion& Region)
{
	return FVector(GetWidthInCells(Region), GetHeightInCells(Region), GetDepthInCells(Region));
}

FRegion URegionHelper::SetLowerCorner(FRegion& Region, const FVector& LowerCorner)
{
	Region.LowerX = (int32)LowerCorner.X;
	Region.LowerY = (int32)LowerCorner.Y;
	Region.LowerZ = (int32)LowerCorner.Z;
	return Region;
}

FRegion URegionHelper::SetUpperCorner(FRegion& Region, const FVector& UpperCorner)
{
	Region.UpperX = UpperCorner.X;
	Region.UpperY = UpperCorner.Y;
	Region.UpperZ = UpperCorner.Z;
	return Region;
}

bool URegionHelper::ContainsFloatPoint(const FRegion& Region, float fX, float fY, float fZ, float boundary /*= 0.0f*/)
{
	return (fX <= Region.UpperX - boundary)
		&& (fY <= Region.UpperY - boundary)
		&& (fZ <= Region.UpperZ - boundary)
		&& (fX >= Region.LowerX + boundary)
		&& (fY >= Region.LowerY + boundary)
		&& (fZ >= Region.LowerZ + boundary);
}

bool URegionHelper::ContainsVectorPoint(const FRegion& Region, const FVector& pos, float boundary /*= 0.0f*/)
{
	return ContainsFloatPoint(Region, pos.X, pos.Y, pos.Z, boundary);
}

bool URegionHelper::ContainsIntPoint(const FRegion& Region, int32 iX, int32 iY, int32 iZ, uint8 boundary /*= 0*/)
{
	return (iX <= Region.UpperX - boundary)
		&& (iY <= Region.UpperY - boundary)
		&& (iZ <= Region.UpperZ - boundary)
		&& (iX >= Region.LowerX + boundary)
		&& (iY >= Region.LowerY + boundary)
		&& (iZ >= Region.LowerZ + boundary);
}

bool URegionHelper::ContainsFloatPointInX(const FRegion& Region, float pos, float boundary /*= 0.0f*/)
{
	return (pos <= Region.UpperX - boundary)
		&& (pos >= Region.LowerX + boundary);
}

bool URegionHelper::ContainsIntPointInX(const FRegion& Region, int32 pos, uint8 boundary /*= 0*/)
{
	return (pos <= Region.UpperX - boundary)
		&& (pos >= Region.LowerX + boundary);
}

bool URegionHelper::ContainsFloatPointInY(const FRegion& Region, float pos, float boundary /*= 0.0f*/)
{
	return (pos <= Region.UpperY - boundary)
		&& (pos >= Region.LowerY + boundary);
}

bool URegionHelper::ContainsIntPointInY(const FRegion& Region, int32 pos, uint8 boundary /*= 0*/)
{
	return (pos <= Region.UpperY - boundary)
		&& (pos >= Region.LowerY + boundary);
}

bool URegionHelper::ContainsFloatPointInZ(const FRegion& Region, float pos, float boundary /*= 0.0f*/)
{
	return (pos <= Region.UpperZ - boundary)
		&& (pos >= Region.LowerZ + boundary);
}

bool URegionHelper::ContainsIntPointInZ(const FRegion& Region, int32 pos, uint8 boundary /*= 0*/)
{
	return (pos <= Region.UpperZ - boundary)
		&& (pos >= Region.LowerZ + boundary);
}

bool URegionHelper::ContainsRegion(const FRegion& Region, const FRegion& other, uint8 boundary /*= 0*/)
{
	return (other.UpperX <= Region.UpperX - boundary)
		&& (other.UpperY <= Region.UpperY - boundary)
		&& (other.UpperZ <= Region.UpperZ - boundary)
		&& (other.LowerX >= Region.LowerX + boundary)
		&& (other.LowerY >= Region.LowerY + boundary)
		&& (other.LowerZ >= Region.LowerZ + boundary);
}

FRegion URegionHelper::AccumulateInt(FRegion& Region, int32 iX, int32 iY, int32 iZ)
{
	Region.LowerX = (FMath::Min(Region.LowerX, iX));
	Region.LowerY = (FMath::Min(Region.LowerY, iY));
	Region.LowerZ = (FMath::Min(Region.LowerZ, iZ));
	Region.UpperX = (FMath::Max(Region.UpperX, iX));
	Region.UpperY = (FMath::Max(Region.UpperY, iY));
	Region.UpperZ = (FMath::Max(Region.UpperZ, iZ));
	return Region;
}

FRegion URegionHelper::AccumulateVector(FRegion& Region, const FVector& v3dPos)
{
	return AccumulateInt(Region, v3dPos.X, v3dPos.Y, v3dPos.Z);
}

FRegion URegionHelper::AccumulateRegion(FRegion& Region, const FRegion& other)
{
	if (!IsValid(other))
	{
		UE_LOG(LogPolyVox, Fatal, TEXT("Tried to accumulate an invalid region! Such behavior is undefined."));
		return Region;
	}

	Region.LowerX = (FMath::Min(Region.LowerX, other.LowerX));
	Region.LowerY = (FMath::Min(Region.LowerY, other.LowerY));
	Region.LowerZ = (FMath::Min(Region.LowerZ, other.LowerZ));
	Region.UpperX = (FMath::Max(Region.UpperX, other.UpperX));
	Region.UpperY = (FMath::Max(Region.UpperY, other.UpperY));
	Region.UpperZ = (FMath::Max(Region.UpperZ, other.UpperZ));
	return Region;
}

FRegion URegionHelper::CropTo(FRegion& Region, const FRegion& other)
{
	Region.LowerX = (FMath::Max(Region.LowerX, other.LowerX));
	Region.LowerY = (FMath::Max(Region.LowerY, other.LowerY));
	Region.LowerZ = (FMath::Max(Region.LowerZ, other.LowerZ));
	Region.UpperX = (FMath::Min(Region.UpperX, other.UpperX));
	Region.UpperY = (FMath::Min(Region.UpperY, other.UpperY));
	Region.UpperZ = (FMath::Min(Region.UpperZ, other.UpperZ));
	return Region;
}

FRegion URegionHelper::GrowConstantAmount(FRegion& Region, int32 Amount)
{
	return GrowByAxis(Region, Amount, Amount, Amount);
}

FRegion URegionHelper::GrowByAxis(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ)
{
	Region.LowerX -= AmountX;
	Region.LowerY -= AmountY;
	Region.LowerZ -= AmountZ;

	Region.UpperX += AmountX;
	Region.UpperY += AmountY;
	Region.UpperZ += AmountZ;

	return Region;
}

FRegion URegionHelper::GrowVector(FRegion& Region, const FVector& Amount)
{
	return GrowByAxis(Region, (int32)Amount.X, (int32)Amount.Y, (int32)Amount.Z);
}

bool URegionHelper::IsValid(const FRegion& Region)
{
	return (Region.UpperX >= Region.LowerX) && (Region.UpperY >= Region.LowerY) && (Region.UpperZ >= Region.LowerZ);
}

FRegion URegionHelper::ShiftInt(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ)
{
	Region = ShiftIntLowerCorner(Region, AmountX, AmountY, AmountZ);
	return ShiftIntUpperCorner(Region, AmountX, AmountY, AmountZ);
}

FRegion URegionHelper::ShiftVector(FRegion& Region, const FVector& Amount)
{
	Region = ShiftVectorLowerCorner(Region, Amount);
	return ShiftVectorUpperCorner(Region, Amount);
}

FRegion URegionHelper::ShiftIntLowerCorner(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ)
{
	Region.LowerX += AmountX;
	Region.LowerY += AmountY;
	Region.LowerZ += AmountZ;
	return Region;
}

FRegion URegionHelper::ShiftVectorLowerCorner(FRegion& Region, const FVector& Amount)
{
	return ShiftIntLowerCorner(Region, (int32)Amount.X, (int32)Amount.Y, (int32)Amount.Z);
}

FRegion URegionHelper::ShiftIntUpperCorner(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ)
{
	Region.UpperX += AmountX;
	Region.UpperY += AmountY;
	Region.UpperZ += AmountZ;
	return Region;
}

FRegion URegionHelper::ShiftVectorUpperCorner(FRegion& Region, const FVector& Amount)
{
	return ShiftIntUpperCorner(Region, (int32)Amount.X, (int32)Amount.Y, (int32)Amount.Z);
}

FRegion URegionHelper::ShrinkConstantAmount(FRegion& Region, int32 Amount)
{
	return GrowConstantAmount(Region, -Amount);
}

FRegion URegionHelper::ShrinkByAxis(FRegion& Region, int32 AmountX, int32 AmountY, int32 AmountZ)
{
	return GrowByAxis(Region, -AmountX, -AmountY, -AmountZ);
}

FRegion URegionHelper::ShrinkVector(FRegion& Region, const FVector& Amount)
{
	return ShrinkByAxis(Region, (int32)Amount.X, (int32)Amount.Y, (int32)Amount.Z);
}

bool URegionHelper::Intersects(const FRegion& Region, const FRegion& Other)
{
	// No intersection if separated along an axis.
	if (GetUpperX(Region) < GetLowerX(Other) || GetLowerX(Region) > GetUpperX(Other)) return false;
	if (GetUpperY(Region) < GetLowerY(Other) || GetLowerY(Region) > GetUpperY(Other)) return false;
	if (GetUpperZ(Region) < GetLowerZ(Other) || GetLowerZ(Region) > GetUpperZ(Other)) return false;

	// Overlapping on all axes means Regions are intersecting.
	return true;
}
