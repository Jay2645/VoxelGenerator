// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "RegionHelper.h"
#include "ArrayHelper.h"

uint8 UArrayHelper::Get2DUint8(const TArray<uint8>& IntArray, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= IntArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return 0;
	}
	return IntArray[index];
}

int32 UArrayHelper::Get2Dint32(const TArray<int32>& IntArray, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= IntArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return 0;
	}
	return IntArray[index];
}

UObject* UArrayHelper::Get2DUObject(const TArray<UObject*>& ObjectArray, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= ObjectArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return 0;
	}
	return ObjectArray[index];
}

FVector UArrayHelper::Get2DFVector(const TArray<FVector>& VectorArray, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= VectorArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return FVector::ZeroVector;
	}
	return VectorArray[index];
}

FColor UArrayHelper::Get2DFColor(const TArray<FColor>& VectorArray, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= VectorArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return FColor::Black;
	}
	return VectorArray[index];
}

float UArrayHelper::Get2DFloat(const TArray<float>& FloatArray, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= FloatArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return 0.0f;
	}
	return FloatArray[index];
}

TArray<uint8> UArrayHelper::Set2DUint8(TArray<uint8>& IntArray, uint8 ValueToSet, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= IntArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return IntArray;
	}
	IntArray[index] = ValueToSet;
	return IntArray;
}

TArray<int32> UArrayHelper::Set2Dint32(TArray<int32>& IntArray, int32 ValueToSet, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= IntArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return IntArray;
	}
	IntArray[index] = ValueToSet;
	return IntArray;
}

TArray<UObject*> UArrayHelper::Set2DUObject(TArray<UObject*>& ObjectArray, UObject* ValueToSet, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= ObjectArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return ObjectArray;
	}
	ObjectArray[index] = ValueToSet;
	return ObjectArray;
}

TArray<FVector> UArrayHelper::Set2DFVector(TArray<FVector>& VectorArray, FVector ValueToSet, const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = Get1DIndexFrom2DIndicies(XPos, YPos, RowLength);
	if (index >= VectorArray.Num())
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was outside the bounds of our array!"));
		return VectorArray;
	}
	VectorArray[index] = ValueToSet;
	return VectorArray;
}

int32 UArrayHelper::Get1DIndexFrom2DIndicies(const int32 XPos, const int32 YPos, const int32 RowLength)
{
	int32 index = XPos + (YPos * RowLength);
	if (index < 0)
	{
		UE_LOG(LogPolyVox, Error, TEXT("Returned an index which was below 0!"));
		return 0;
	}
	return index;
}
