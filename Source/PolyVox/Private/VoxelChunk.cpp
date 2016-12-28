// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "Wrappers/VoxelVolume.h"
#include "VoxelChunk.h"

AVoxelChunk::AVoxelChunk()
{
	TerrainMesh = CreateDefaultSubobject<UVoxelProceduralMeshComponent>(TEXT("Terrain Mesh"));
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

AVoxelChunk* AVoxelChunk::GenerateTerrain(AVoxelVolume* VoxelVolume, URegion* OurRegion)
{
	UChunkManager* chunkManager = UChunkManager::GetManager(VoxelVolume->GetWorld());
	
	AVoxelChunk* chunk = (AVoxelChunk*)VoxelVolume->GetWorld()->SpawnActor(AVoxelChunk::StaticClass());

	chunk->ChunkPosition = OurRegion->GetCenter();
	chunk->SetActorLocation(chunk->ChunkPosition * chunkManager->VoxelSize);
	int32 chunkSize = chunkManager->ChunkSize;
	chunk->SetChunkID(OurRegion->GetLowerX() / chunkSize, OurRegion->GetLowerY() / chunkSize);

	chunk->ChunkRegion = OurRegion;
	VoxelVolume->DrawDebugChunk(OurRegion);
	//chunk->TerrainMesh->CreateMarchingCubesMeshFromVolume(VoxelVolume, chunk->ChunkRegion, chunkManager->TerrainMaterials);
	return chunk;
}

URegion* AVoxelChunk::GetChunkRegion() const
{
	return ChunkRegion;
}

void AVoxelChunk::AddTreeLocation(FName Biome, FVector ChunkPoint)
{
	TArray<FVector> treeArray = PossibleTreeLocations.FindOrAdd(Biome);
	treeArray.Add(ChunkPoint);
	PossibleTreeLocations[Biome] = treeArray;
}