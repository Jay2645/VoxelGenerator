#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "ArrayHelper.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class POLYVOX_API UArrayHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Arrays")
	static uint8 Get2DUint8(const TArray<uint8>& IntArray, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static FGameplayTag Get2DGameplayTag(const TArray<FGameplayTag>& IntArray, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static int32 Get2Dint32(const TArray<int32>& IntArray, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static UObject* Get2DUObject(const TArray<UObject*>& ObjectArray, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static FVector Get2DFVector(const TArray<FVector>& VectorArray, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static FColor Get2DFColor(const TArray<FColor>& VectorArray, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static float Get2DFloat(const TArray<float>& FloatArray, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static TArray<uint8> Set2DUint8(TArray<uint8>& IntArray, uint8 ValueToSet, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static TArray<int32> Set2Dint32(TArray<int32>& IntArray, int32 ValueToSet, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static TArray<UObject*> Set2DUObject(TArray<UObject*>& ObjectArray, UObject* ValueToSet, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static TArray<FVector> Set2DFVector(TArray<FVector>& VectorArray, FVector ValueToSet, const int32 XPos, const int32 YPos, const int32 RowLength);

	UFUNCTION(BlueprintPure, Category = "Arrays")
	static int32 Get1DIndexFrom2DIndicies(const int32 XPos, const int32 YPos, const int32 RowLength);
};
