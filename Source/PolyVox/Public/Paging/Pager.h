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

#pragma once

#include "UObject/NoExportTypes.h"
#include "RegionHelper.h"
#include "Pager.generated.h"

class APagedChunk;

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
	void PageIn(const FRegion& Region, APagedChunk* Chunk);
	UFUNCTION(BlueprintNativeEvent, Category = "Pager")
	void PageOut(const FRegion& Region, APagedChunk* Chunk);

protected:
	virtual void PageIn_Implementation(const FRegion& Region, APagedChunk* Chunk);
	virtual void PageOut_Implementation(const FRegion& Region, APagedChunk* Chunk);
};
