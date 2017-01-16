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
#include "Utils/Morton.h"
#include "Paging/PagedVolume.h"
#include "Paging/PagedChunk.h"
#include <array>
#include "VolumeSampler.h"

#define CAN_GO_NEG_X(val) (val > 0)
#define CAN_GO_POS_X(val)  (val < this->ChunkSideLengthMinusOne)
#define CAN_GO_NEG_Y(val) (val > 0)
#define CAN_GO_POS_Y(val)  (val < this->ChunkSideLengthMinusOne)
#define CAN_GO_NEG_Z(val) (val > 0)
#define CAN_GO_POS_Z(val)  (val < this->ChunkSideLengthMinusOne)

#define NEG_X_DELTA (-(deltaX[this->XPosInChunk-1]))
#define POS_X_DELTA (deltaX[this->XPosInChunk])
#define NEG_Y_DELTA (-(deltaY[this->YPosInChunk-1]))
#define POS_Y_DELTA (deltaY[this->YPosInChunk])
#define NEG_Z_DELTA (-(deltaZ[this->ZPosInChunk-1]))
#define POS_Z_DELTA (deltaZ[this->ZPosInChunk])

static const std::array<int32, 256> deltaX = { 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 224695, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 1797559, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 224695, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 28087, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1, 3511, 1, 7, 1, 55, 1, 7, 1, 439, 1, 7, 1, 55, 1, 7, 1 };
static const std::array<int32, 256> deltaY = { 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 449390, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 3595118, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 449390, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 56174, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2, 7022, 2, 14, 2, 110, 2, 14, 2, 878, 2, 14, 2, 110, 2, 14, 2 };
static const std::array<int32, 256> deltaZ = { 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 898780, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 7190236, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 898780, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 112348, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4, 14044, 4, 28, 4, 220, 4, 28, 4, 1756, 4, 28, 4, 220, 4, 28, 4 };


UVolumeSampler::UVolumeSampler(APagedVolume* VolumeData)
{
	Volume = VolumeData;
}

UVolumeSampler::UVolumeSampler(const UVolumeSampler& Sampler)
{
	Volume = Sampler.Volume;

	XPosInVolume = Sampler.XPosInVolume;
	YPosInVolume = Sampler.YPosInVolume;
	ZPosInVolume = Sampler.ZPosInVolume;

	XPosInChunk = Sampler.XPosInChunk;
	YPosInChunk = Sampler.YPosInChunk;
	ZPosInChunk = Sampler.ZPosInChunk;

	CurrentVoxelIndex = Sampler.CurrentVoxelIndex;
	CurrentChunk = Sampler.CurrentChunk;
}

UVoxel* UVolumeSampler::GetVoxel()
{
	uint8 sideLengthPower = Volume->GetSideLengthPower();
	if (CurrentChunk == NULL)
	{
		UE_LOG(LogPolyVox, Log, TEXT("Current chunk was null. Getting by coordinates."));
		return Volume->GetVoxelByCoordinates(XPosInVolume, YPosInVolume, ZPosInVolume);
	}
	else
	{
		TArray<UVoxel*> data = CurrentChunk->GetData();
		if (CurrentVoxelIndex < 0 || CurrentVoxelIndex >= data.Num())
		{
			UE_LOG(LogPolyVox, Warning, TEXT("Current voxel index %d was out of range!"), CurrentVoxelIndex);
			return UVoxel::GetEmptyVoxel();
		}
		if (data[CurrentVoxelIndex] == NULL)
		{
			return UVoxel::GetEmptyVoxel();
		}
		return data[CurrentVoxelIndex];
	}
}

void UVolumeSampler::SetPosition(int32 XPos, int32 YPos, int32 ZPos)
{
	if (Volume == NULL)
	{
		UE_LOG(LogPolyVox, Fatal, TEXT("Sampler volume was null!"));
		return;
	}
	XPosInVolume = XPos;
	YPosInVolume = YPos;
	ZPosInVolume = ZPos;

	// Then we update the voxel pointer
	uint8 sideLengthPower = Volume->GetSideLengthPower();
	if (sideLengthPower > 0)
	{
		const int32 xChunk = XPosInVolume >> sideLengthPower;
		const int32 yChunk = YPosInVolume >> sideLengthPower;
		const int32 zChunk = ZPosInVolume >> sideLengthPower;

		XPosInChunk = (XPosInVolume - (xChunk << Volume->GetSideLengthPower()));
		YPosInChunk = (YPosInVolume - (yChunk << Volume->GetSideLengthPower()));
		ZPosInChunk = (ZPosInVolume - (zChunk << Volume->GetSideLengthPower()));

		uint32 voxelIndexInChunk = morton256_x[XPosInChunk] | morton256_y[YPosInChunk] | morton256_z[ZPosInChunk];

		CurrentChunk = Volume->CanReuseLastAccessedChunk(xChunk, yChunk, zChunk) ? Volume->LastAccessedChunk : Volume->GetChunk(xChunk, yChunk, zChunk);

		CurrentVoxelIndex = voxelIndexInChunk;
	}
}


void UVolumeSampler::MoveNegativeX()
{
	XPosInVolume--;

	// Then we update the voxel pointer
	if (CAN_GO_NEG_X(XPosInChunk))
	{
		//No need to compute new chunk.
		CurrentVoxelIndex += NEG_X_DELTA;
		XPosInChunk--;
	}
	else
	{
		//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
		SetPosition(XPosInVolume, YPosInVolume, ZPosInVolume);
	}
}

void UVolumeSampler::MovePositiveX()
{
	XPosInVolume++;

	// Then we update the voxel pointer
	if (CAN_GO_POS_X(XPosInChunk))
	{
		//No need to compute new chunk.
		CurrentVoxelIndex += POS_X_DELTA;
		XPosInChunk++;
	}
	else
	{
		//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
		SetPosition(XPosInVolume, YPosInVolume, ZPosInVolume);
	}
}

void UVolumeSampler::MoveNegativeY()
{
	YPosInVolume--;

	// Then we update the voxel pointer
	if (CAN_GO_NEG_Y(YPosInChunk))
	{
		//No need to compute new chunk.
		CurrentVoxelIndex += NEG_Y_DELTA;
		YPosInChunk--;
	}
	else
	{
		//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
		SetPosition(XPosInVolume, YPosInVolume, ZPosInVolume);
	}
}

void UVolumeSampler::MovePositiveY()
{
	YPosInVolume++;

	// Then we update the voxel pointer
	if (CAN_GO_POS_Y(YPosInChunk))
	{
		//No need to compute new chunk.
		CurrentVoxelIndex += POS_Y_DELTA;
		YPosInChunk++;
	}
	else
	{
		//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
		SetPosition(XPosInVolume, YPosInVolume, ZPosInVolume);
	}
}

void UVolumeSampler::MoveNegativeZ()
{
	ZPosInVolume--;

	// Then we update the voxel pointer
	if (CAN_GO_NEG_Z(ZPosInChunk))
	{
		//No need to compute new chunk.
		CurrentVoxelIndex += NEG_Z_DELTA;
		ZPosInChunk--;
	}
	else
	{
		//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
		SetPosition(XPosInVolume, YPosInVolume, ZPosInVolume);
	}
}

void UVolumeSampler::MovePositiveZ()
{
	ZPosInVolume++;
	// Then we update the voxel pointer
	if (CAN_GO_POS_Z(ZPosInChunk))
	{
		//No need to compute new chunk.
		CurrentVoxelIndex += POS_Z_DELTA;
		ZPosInChunk++;
	}
	else
	{
		//We've hit the chunk boundary. Just calling setPosition() is the easiest way to resolve this.
		SetPosition(XPosInVolume, YPosInVolume, ZPosInVolume);
	}
}