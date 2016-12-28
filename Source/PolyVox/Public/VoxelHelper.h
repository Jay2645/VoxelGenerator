// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PolyVox/Vector.h"
#include "Components/StaticMeshComponent.h"
#include "Noise/FastNoise.h"
#include "VoxelHelper.generated.h"

// Bridge between PolyVox Vector3DFloat and Unreal Engine 4 FVector
struct FPolyVoxVector : public FVector
{
	UPROPERTY()
		FORCEINLINE FPolyVoxVector()
	{}

	explicit FORCEINLINE FPolyVoxVector(EForceInit E)
		: FVector(E)
	{}

	FORCEINLINE FPolyVoxVector(float InX, float InY, float InZ)
		: FVector(InX, InY, InX)
	{}

	FORCEINLINE FPolyVoxVector(const FVector &InVec)
	{
		FVector::operator=(InVec);
	}

	FORCEINLINE FPolyVoxVector(const PolyVox::Vector3DFloat &InVec)
	{
		FPolyVoxVector::operator=(InVec);
	}

	FORCEINLINE FVector& operator=(const PolyVox::Vector3DFloat& Other)
	{
		this->X = Other.getX();
		this->Y = Other.getY();
		this->Z = Other.getZ();

		DiagnosticCheckNaN();

		return *this;
	}

	FORCEINLINE FPolyVoxVector(const PolyVox::Vector3DInt32 &InVec)
	{
		FPolyVoxVector::operator=(InVec);
	}

	FORCEINLINE FVector& operator=(const PolyVox::Vector3DInt32& Other)
	{
		this->X = Other.getX();
		this->Y = Other.getY();
		this->Z = Other.getZ();

		DiagnosticCheckNaN();

		return *this;
	}
};

USTRUCT(BlueprintType)
struct FVoxelTriangle
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		FVector Vertex0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		FVector Vertex1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		FVector Vertex2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		int32 MaterialID;
};

USTRUCT(BlueprintType)
struct FVoxelMeshSection
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		TArray<FVoxelTriangle> Triangles;
};

USTRUCT(BlueprintType)
struct POLYVOX_API FVoxelHeightmapData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FName Biome;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	float Elevation;
};

USTRUCT(BlueprintType)
struct POLYVOX_API FRandomTreeData
{
	GENERATED_BODY()

		UPROPERTY(Category = "Tree", BlueprintReadWrite, EditAnywhere)
		UStaticMesh* TreeMesh;

	UPROPERTY(Category = "Tree", BlueprintReadWrite, EditAnywhere)
		float TreeSelectionChance;

	//Constructor
	FRandomTreeData()
	{
		TreeSelectionChance = 1.0f;
	}
};

USTRUCT(BlueprintType)
struct FVoxelMaterial
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
		UMaterialInterface* Material;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
		bool bShouldCreateCollision;
};

USTRUCT(BlueprintType)
struct POLYVOX_API FNoiseProfile
{
	GENERATED_BODY()

		UPROPERTY(Category = "Voxel Biome", BlueprintReadWrite, EditAnywhere)
		FName ProfileName;

	UPROPERTY(Category = "Voxel Biome", BlueprintReadWrite, EditAnywhere)
		int32 MaterialIndex;

	UPROPERTY(Category = "Voxel Terrain", BlueprintReadWrite, EditAnywhere)
		FNoiseSettings NoiseSettings;

	// How "squished" the noise is
	UPROPERTY(Category = "Voxel Terrain", BlueprintReadWrite, EditAnywhere)
		float TerrainHeightScale;

	UPROPERTY(Category = "Voxel Terrain", BlueprintReadWrite, EditAnywhere)
		float NoiseVerticalOffset;

	UPROPERTY(Category = "Foilage", BlueprintReadWrite, EditAnywhere)
		TArray<FRandomTreeData> TreeMeshes;

	//Constructor
	FNoiseProfile()
	{
		MaterialIndex = 0;
		TerrainHeightScale = 0.5f;
		NoiseVerticalOffset = 0.0f;
		ProfileName = NAME_None;
	}
};