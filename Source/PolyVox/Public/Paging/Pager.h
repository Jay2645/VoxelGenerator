// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "RegionHelper.h"
#include "Pager.generated.h"

class UPagedChunk;

/** 
* Users can override this class and provide an instance of the derived class to the PagedVolume constructor. This derived class
* could then perform tasks such as compression and decompression of the data, and read/writing it to a file, database, network,
* or other storage as appropriate.
*/
UCLASS(Blueprintable)
class POLYVOX_API UPager : public UObject
{
	GENERATED_BODY()
public:
	/// Constructor
	UPager() {};
	/// Destructor
	virtual ~UPager() {};

	UFUNCTION(BlueprintNativeEvent, Category = "Pager")
	void PageIn(const FRegion& Region, UPagedChunk* Chunk);
	UFUNCTION(BlueprintNativeEvent, Category = "Pager")
	void PageOut(const FRegion& Region, UPagedChunk* Chunk);

protected:
	virtual void PageIn_Implementation(const FRegion& Region, UPagedChunk* Chunk);
	virtual void PageOut_Implementation(const FRegion& Region, UPagedChunk* Chunk);
};
