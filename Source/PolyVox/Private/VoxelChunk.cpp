// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "Pagers/VoxelInfiniteTerrainPager.h"
#include "VoxelChunk.h"

// PolyVox includes
#include "PolyVox/MarchingCubesSurfaceExtractor.h"

AVoxelChunk::AVoxelChunk()
{
	TerrainMesh = CreateDefaultSubobject<UVoxelProceduralMeshComponent>(TEXT("Terrain Mesh"));
}

void AVoxelChunk::BeginPlay()
{
	GenerateTerrain();
}

void AVoxelChunk::SetChunkID(const int32& idX, const int32& idY)
{
	XID = idX;
	YID = idY;
	this->Rename(*(FString("VoxelTerrain") + FString::FromInt(XID) + "x" + FString::FromInt(YID)));
}

FVector AVoxelChunk::VoxelPositionToWorldPosition(int32 x, int32 y, int32 z) const
{
	float voxelSize = UChunkManager::GetManager(GetWorld())->VoxelSize;
	FVector localPosition = FVector(x * voxelSize, y * voxelSize, (z * voxelSize) - 3150.0f);
	localPosition.X += ChunkPosition.X * voxelSize;
	localPosition.Y += ChunkPosition.Y * voxelSize;
	return localPosition;
}

void AVoxelChunk::GenerateTerrain()
{
	UChunkManager* chunkManager = UChunkManager::GetManager(GetWorld());
	int32 chunkSize = chunkManager->ChunkSize;
	int32 startXLocation = XID * chunkSize;
	int32 startYLocation = YID * chunkSize;
	int32 endXLocation = startXLocation + chunkSize - 1;
	int32 endYLocation = startYLocation + chunkSize - 1;
	ChunkRegion = PolyVox::Region(Vector3DInt32(startXLocation, startYLocation, 0), Vector3DInt32(endXLocation, endYLocation, chunkManager->ChunkHeight));
	TerrainMesh->CreateMeshFromVolume(new PagedVolume<MaterialDensityPair44>(new VoxelInfiniteTerrainPager(GetActorLocation(),this)),ChunkRegion, chunkManager->TerrainMaterials);
}

PolyVox::Region AVoxelChunk::GetChunkRegion() const
{
	return ChunkRegion;
}

void AVoxelChunk::AddTreeLocation(FName Biome, FVector ChunkPoint)
{
	TArray<FVector> treeArray = PossibleTreeLocations.FindOrAdd(Biome);
	treeArray.Add(ChunkPoint);
	PossibleTreeLocations[Biome] = treeArray;
}