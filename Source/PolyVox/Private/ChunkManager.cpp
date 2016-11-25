// Fill out your copyright notice in the Description page of Project Settings.

#include "PolyVoxPrivatePCH.h"
#include "ChunkManager.h"


// Sets default values for this component's properties
UChunkManager::UChunkManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


UChunkManager* UChunkManager::GetManager(UWorld* World)
{
	AGameStateBase* gameState = World->GetGameState();
	UChunkManager* manager = (UChunkManager*)gameState->GetComponentByClass(UChunkManager::StaticClass());
	if (manager == NULL)
	{
		manager = CreateManager(gameState);
	}
	return manager;
}

UChunkManager* UChunkManager::CreateManager(AGameStateBase* gameState)
{
	return NewObject<UChunkManager>(gameState, UChunkManager::StaticClass());
}