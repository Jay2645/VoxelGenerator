// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "VoxelHelper.h"
#include "ChunkManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYVOX_API UChunkManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChunkManager();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		TArray<FVoxelMaterial> TerrainMaterials;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		TArray<FNoiseProfile> TerrainProfiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
		float WaterHeight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxels")
	float VoxelSize = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxels")
	int32 ChunkSize = 128;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxels")
	int32 ChunkHeight = 64;


public:
	static UChunkManager* GetManager(UWorld* World);
private:
	static UChunkManager* CreateManager(AGameStateBase* gameState);
};