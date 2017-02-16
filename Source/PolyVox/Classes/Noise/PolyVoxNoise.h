// MIT License
//
// Copyright(c) 2016 Jordan Peck
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The developer's email is jorzixdan.me2@gzixmail.com (for great email, take
// off every 'zix'.)
//

#pragma once

#include "UObject/NoExportTypes.h"
#include "PolyVoxNoise.generated.h"

//namespace VoxelGenerator
//{
	UENUM()		//"BlueprintType" is essential to include
	enum class EVoxelNoiseType : uint8
	{
		Value 			UMETA(DisplayName = "Value"),
		ValueFractal 	UMETA(DisplayName = "ValueFractal"),
		Gradient		UMETA(DisplayName = "Gradient"),
		GradientFractal UMETA(DisplayName = "GradientFractal"),
		Simplex 		UMETA(DisplayName = "Simplex"),
		SimplexFractal	UMETA(DisplayName = "SimplexFractal"),
		Cellular 		UMETA(DisplayName = "Cellular"),
		WhiteNoise		UMETA(DisplayName = "WhiteNoise")
	};

	UENUM()		//"BlueprintType" is essential to include
	enum class EVoxelInterp : uint8
	{
		InterpLinear 	UMETA(DisplayName = "InterpLinear"),
		InterpHermite 	UMETA(DisplayName = "InterpHermite"),
		InterpQuintic	UMETA(DisplayName = "InterpQuintic")
	};

	UENUM()		//"BlueprintType" is essential to include
	enum class EVoxelFractalType : uint8
	{
		FBM 			UMETA(DisplayName = "FBM"),
		Billow 			UMETA(DisplayName = "Billow"),
		RigidMulti		UMETA(DisplayName = "RigidMulti")
	};


	UENUM()		//"BlueprintType" is essential to include
	enum class EVoxelCellularDistanceFunction : uint8
	{
		Euclidean 		UMETA(DisplayName = "Euclidean"),
		Manhattan 		UMETA(DisplayName = "Manhattan"),
		Natural			UMETA(DisplayName = "Natural")
	};

	UENUM()		//"BlueprintType" is essential to include
	enum class EVoxelCellularReturnType : uint8
	{
		CellValue 		UMETA(DisplayName = "CellValue"),
		NoiseLookup 	UMETA(DisplayName = "NoiseLookup"),
		Distance		UMETA(DisplayName = "Distance"),
		Distance2		UMETA(DisplayName = "Distance2"),
		Distance2Add 	UMETA(DisplayName = "Distance2Add"),
		Distance2Sub	UMETA(DisplayName = "Distance2Sub"),
		Distance2Mul 	UMETA(DisplayName = "Distance2Mul"),
		Distance2Div	UMETA(DisplayName = "Distance2Div")
	};

	USTRUCT()
	struct FVoxelNoiseSettings
	{
		GENERATED_BODY()

			// The seed of our fractal
			UPROPERTY(Category = "Noise Settings", BlueprintReadWrite, EditAnywhere)
			int32 Seed;

		// The number of octaves that the noise generator will use. More octaves creates "messier" noise.
		UPROPERTY(Category = "Noise Settings", BlueprintReadWrite, EditAnywhere)
			int32 NoiseOctaves;

		// The frequency of the noise
		UPROPERTY(Category = "Noise Settings", BlueprintReadWrite, EditAnywhere)
			float NoiseFrequency;

		// The detail found in the noise
		UPROPERTY(Category = "Noise Settings", BlueprintReadWrite, EditAnywhere)
			float Lacunarity;

		// The contrast of the noise
		UPROPERTY(Category = "Noise Settings", BlueprintReadWrite, EditAnywhere)
			float Gain;

		// The type of fractal to use to generate the noise
		UPROPERTY(Category = "Noise Settings", BlueprintReadWrite, EditAnywhere)
			EVoxelFractalType FractalType;

		// The type of noise to use
		UPROPERTY(Category = "Noise Settings", BlueprintReadWrite, EditAnywhere)
			EVoxelNoiseType NoiseType;

		// How to interpolate the noise
		UPROPERTY(Category = "Noise Settings", BlueprintReadWrite, EditAnywhere)
			EVoxelInterp NoiseInterpolation;

		//Constructor
		FVoxelNoiseSettings()
		{
			Seed = 123;
			NoiseOctaves = 3;
			NoiseFrequency = 0.01f;
			Lacunarity = 2.0f;
			Gain = 0.5f;
			NoiseType = EVoxelNoiseType::SimplexFractal;
			NoiseInterpolation = EVoxelInterp::InterpLinear;
			FractalType = EVoxelFractalType::FBM;
		}
	};

	USTRUCT(BlueprintType)
	struct FRandomTreeData
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
	struct FVoxelNoiseProfile
	{
		GENERATED_BODY()

			UPROPERTY(Category = "Voxel Biome", BlueprintReadWrite, EditAnywhere)
			FName ProfileName;

		UPROPERTY(Category = "Voxel Biome", BlueprintReadWrite, EditAnywhere)
			int32 MaterialIndex;

		UPROPERTY(Category = "Voxel Terrain", BlueprintReadWrite, EditAnywhere)
			FVoxelNoiseSettings NoiseSettings;

		// How "squished" the noise is
		UPROPERTY(Category = "Voxel Terrain", BlueprintReadWrite, EditAnywhere)
			float TerrainHeightScale;

		UPROPERTY(Category = "Voxel Terrain", BlueprintReadWrite, EditAnywhere)
			float NoiseVerticalOffset;

		UPROPERTY(Category = "Foilage", BlueprintReadWrite, EditAnywhere)
			TArray<FRandomTreeData> TreeMeshes;

		//Constructor
		FVoxelNoiseProfile()
		{
			MaterialIndex = 0;
			TerrainHeightScale = 0.5f;
			NoiseVerticalOffset = 0.0f;
			ProfileName = NAME_None;
		}
	};


	/**
	 * 
	 */
	class PolyVoxNoise
	{
	public:
		PolyVoxNoise(int seed = 1337) { SetSeed(seed); };
		~PolyVoxNoise() { delete m_cellularNoiseLookup; }

		//enum EVoxelNoiseType { Value, ValueFractal, Gradient, GradientFractal, Simplex, SimplexFractal, Cellular, WhiteNoise };
		//enum EVoxelInterp { InterpLinear, InterpHermite, InterpQuintic };
		//enum EVoxelFractalType { FBM, Billow, RigidMulti };
		//enum EVoxelCellularDistanceFunction { Euclidean, Manhattan, Natural };
		//enum EVoxelCellularReturnType { CellValue, NoiseLookup, Distance, Distance2, Distance2Add, Distance2Sub, Distance2Mul, Distance2Div };

		void SetSeed(int seed);
		int GetSeed(void) const { return m_seed; }
		void SetNoiseSettings(FVoxelNoiseSettings m_noiseSettings);
		void SetFrequency(float frequency) { m_frequency = frequency; }
		void SetInterp(EVoxelInterp interp) { m_interp = interp; }
		void SetNoiseType(EVoxelNoiseType noiseType) { m_noiseType = noiseType; }

		void SetFractalOctaves(unsigned int octaves) { m_octaves = octaves; }
		void SetFractalLacunarity(float lacunarity) { m_lacunarity = lacunarity; }
		void SetFractalGain(float gain) { m_gain = gain; }
		void SetFractalType(EVoxelFractalType fractalType) { m_fractalType = fractalType; }

		void SetCellularDistanceFunction(EVoxelCellularDistanceFunction cellularDistanceFunction) { m_cellularDistanceFunction = cellularDistanceFunction; }
		void SetCellularReturnType(EVoxelCellularReturnType cellularReturnType) { m_cellularReturnType = cellularReturnType; }
		void SetCellularNoiseLookup(PolyVoxNoise* noise) { m_cellularNoiseLookup = noise; }

		/*
		Timing below are averages of time taken for 1 million iterations on a single thread
		Default noise settings
		CPU: i7 4790k @ 4.0Ghz
		VS 2013 - C++ Console Application
		*/

		//3D												// Win32	x64
		float GetValue(float x, float y, float z);			// 14 ms	14 ms
		float GetValueFractal(float x, float y, float z);	// 48 ms	49 ms

		float GetGradient(float x, float y, float z);		// 23 ms	22 ms
		float GetGradientFractal(float x, float y, float z);// 80 ms	73 ms

		float GetSimplex(float x, float y, float z);		// 30 ms	30 ms
		float GetSimplexFractal(float x, float y, float z);	// 98 ms	101 ms

		float GetCellular(float x, float y, float z);		// 123 ms	113 ms

		float GetWhiteNoise(float x, float y, float z);		// 1.5 ms	1.5 ms
		float GetWhiteNoiseInt(int x, int y, int z);

		float GetNoise(float x, float y, float z);

		//2D												// Win32	x64
		float GetValue(float x, float y);					// 8 ms 	8 ms
		float GetValueFractal(float x, float y);			// 29 ms	29 ms

		float GetGradient(float x, float y);				// 12 ms	11 ms
		float GetGradientFractal(float x, float y);			// 43 ms	40 ms

		float GetSimplex(float x, float y);					// 17 ms	17 ms
		float GetSimplexFractal(float x, float y);			// 55 ms	52 ms

		float GetCellular(float x, float y);				// 35 ms	33 ms

		float GetWhiteNoise(float x, float y);				// 1 ms		1 ms
		float GetWhiteNoiseInt(int x, int y);				// 1 ms		1 ms

		float GetNoise(float x, float y);

		//4D
		float GetSimplex(float x, float y, float z, float w);

		float GetWhiteNoise(float x, float y, float z, float w);
		float GetWhiteNoiseInt(int x, int y, int z, int w);

	protected:
		unsigned char m_perm[512];
		unsigned char m_perm12[512];

		int m_seed = 1337;
		float m_frequency = 0.01f;
		EVoxelInterp m_interp = EVoxelInterp::InterpQuintic;
		EVoxelNoiseType m_noiseType = EVoxelNoiseType::Simplex;

		unsigned int m_octaves = 3;
		float m_lacunarity = 2.0f;
		float m_gain = 0.5f;
		EVoxelFractalType m_fractalType = EVoxelFractalType::FBM;

		EVoxelCellularDistanceFunction m_cellularDistanceFunction = EVoxelCellularDistanceFunction::Euclidean;
		EVoxelCellularReturnType m_cellularReturnType = EVoxelCellularReturnType::CellValue;
		PolyVoxNoise* m_cellularNoiseLookup = nullptr;

		//2D
		float SingleValueFractalFBM(float x, float y);
		float SingleValueFractalBillow(float x, float y);
		float SingleValueFractalRigidMulti(float x, float y);
		float SingleValue(unsigned char offset, float x, float y);

		float SingleGradientFractalFBM(float x, float y);
		float SingleGradientFractalBillow(float x, float y);
		float SingleGradientFractalRigidMulti(float x, float y);
		float SingleGradient(unsigned char offset, float x, float y);

		float SingleSimplexFractalFBM(float x, float y);
		float SingleSimplexFractalBillow(float x, float y);
		float SingleSimplexFractalRigidMulti(float x, float y);
		float SingleSimplex(unsigned char offset, float x, float y);

		float SingleCellular(float x, float y);
		float SingleCellular2Edge(float x, float y);

		//3D
		float SingleValueFractalFBM(float x, float y, float z);
		float SingleValueFractalBillow(float x, float y, float z);
		float SingleValueFractalRigidMulti(float x, float y, float z);
		float SingleValue(unsigned char offset, float x, float y, float z);

		float SingleGradientFractalFBM(float x, float y, float z);
		float SingleGradientFractalBillow(float x, float y, float z);
		float SingleGradientFractalRigidMulti(float x, float y, float z);
		float SingleGradient(unsigned char offset, float x, float y, float z);

		float SingleSimplexFractalFBM(float x, float y, float z);
		float SingleSimplexFractalBillow(float x, float y, float z);
		float SingleSimplexFractalRigidMulti(float x, float y, float z);
		float SingleSimplex(unsigned char offset, float x, float y, float z);

		float SingleCellular(float x, float y, float z);
		float SingleCellular2Edge(float x, float y, float z);

		//4D
		float SingleSimplex(unsigned char offset, float x, float y, float z, float w);

	private:
		inline unsigned char Index2D_12(unsigned char offset, int x, int y);
		inline unsigned char Index3D_12(unsigned char offset, int x, int y, int z);
		inline unsigned char Index4D_32(unsigned char offset, int x, int y, int z, int w);
		inline unsigned char Index2D_256(unsigned char offset, int x, int y);
		inline unsigned char Index3D_256(unsigned char offset, int x, int y, int z);
		inline unsigned char Index4D_256(unsigned char offset, int x, int y, int z, int w);

		inline float ValCoord2DFast(unsigned char offset, int x, int y);
		inline float ValCoord3DFast(unsigned char offset, int x, int y, int z);
		inline float GradCoord2D(unsigned char offset, int x, int y, float xd, float yd);
		inline float GradCoord3D(unsigned char offset, int x, int y, int z, float xd, float yd, float zd);
		inline float GradCoord4D(unsigned char offset, int x, int y, int z, int w, float xd, float yd, float zd, float wd);
	};
//}