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
#include "ArrayHelper.h"
#include "DrawDebugHelpers.h"
#include "VoxelProceduralMeshComponent.h"

#define DO_CHECK = 1

void UVoxelProceduralMeshComponent::CreateMarchingCubesMesh(ABaseVolume* VolumeData, FRegion Region, const TArray<FVoxelMaterial>& VoxelMaterials)
{
	auto rawMesh = GetEncodedMesh(VolumeData, Region, UMarchingCubesDefaultController::StaticClass());
	TArray<FVoxelMeshSection> meshSections = GenerateTriangles(rawMesh);
	if (meshSections.Num() > VoxelMaterials.Num())
	{
		UE_LOG(LogPolyVox, Warning, TEXT("More mesh sections are being made (%d) than there are materials defined (%d)."), meshSections.Num(), VoxelMaterials.Num());
		return;
	}
	else if (meshSections.Num() == 0)
	{
		UE_LOG(LogPolyVox, Error, TEXT("No meshes defined when sending volume to procedural mesh!"));
		return;
	}
	for (int i = 0; i < meshSections.Num(); i++)
	{
		FProcMeshSection meshSection = CreateMeshSectionData(meshSections[i].Triangles, VoxelMaterials[i].bShouldCreateCollision);
		SetProcMeshSection(i, meshSection);
		if (VoxelMaterials.Num() > i)
		{
			SetMaterial(i, VoxelMaterials[i].Material);
		}
		else
		{
			SetMaterial(i, VoxelMaterials[0].Material);
		}
	}
}

FVoxelMesh UVoxelProceduralMeshComponent::AddVertex(FVoxelMesh& VoxelMesh, const FVoxelVertex& Vertex)
{
	VoxelMesh.Vertices.Add(Vertex);
	return VoxelMesh;
}

FVoxelMesh UVoxelProceduralMeshComponent::AddTriangle(FVoxelMesh& VoxelMesh, const int32& Index0, const int32& Index1, const int32& Index2)
{
	checkf(Index0 < VoxelMesh.Vertices.Num(), TEXT("Index points at an invalid vertex."));
	checkf(Index1 < VoxelMesh.Vertices.Num(), TEXT("Index points at an invalid vertex."));
	checkf(Index2 < VoxelMesh.Vertices.Num(), TEXT("Index points at an invalid vertex."));

	VoxelMesh.Indices.Add(Index0);
	VoxelMesh.Indices.Add(Index1);
	VoxelMesh.Indices.Add(Index2);

	return VoxelMesh;
}

FProcMeshSection UVoxelProceduralMeshComponent::CreateMeshSectionData(TArray<FVoxelTriangle> Triangles, bool bShouldEnableCollision)
{
	FProcMeshSection meshSection;
	float voxelSize = 100.0f;

	meshSection.ProcVertexBuffer.Reset();
	for (int i = 0; i < Triangles.Num(); i++)
	{
		FProcMeshVertex vertex2;
		FProcMeshVertex vertex1;
		FProcMeshVertex vertex0;

		vertex0.Position = Triangles[i].Vertex0.Position * voxelSize;
		vertex1.Position = Triangles[i].Vertex1.Position * voxelSize;
		vertex2.Position = Triangles[i].Vertex2.Position * voxelSize;

		// Calculate the tangents of our triangle
		const FVector Edge01 = FVector(Triangles[i].Vertex1.Position - Triangles[i].Vertex0.Position);
		const FVector Edge02 = FVector(Triangles[i].Vertex2.Position - Triangles[i].Vertex0.Position);
		const FVector TangentX = Edge01.GetSafeNormal();
		FVector TangentZ = (Edge01 ^ Edge02).GetSafeNormal();

		// Set Vertex tangents and normals
		vertex0.Tangent = FProcMeshTangent(TangentX, false);
		vertex1.Tangent = FProcMeshTangent(TangentX, false);
		vertex2.Tangent = FProcMeshTangent(TangentX, false);
		vertex0.Normal = TangentZ;
		vertex1.Normal = TangentZ;
		vertex2.Normal = TangentZ;

		// UVs
		vertex0.UV0 = FVector2D(0.0f, 0.0f);
		vertex1.UV0 = FVector2D(0.0f, 0.0f);
		vertex2.UV0 = FVector2D(0.0f, 0.0f);

		// Vertex Colors
		vertex0.Color = FColor(255, 255, 255);
		vertex1.Color = FColor(255, 255, 255);
		vertex2.Color = FColor(255, 255, 255);

		// Update bounding box
		meshSection.SectionLocalBox += vertex0.Position;
		meshSection.SectionLocalBox += vertex1.Position;
		meshSection.SectionLocalBox += vertex2.Position;

		// Add to vertex buffer
		// We need to add the vertices of each triangle in reverse or the mesh will be upside down
		int32 index2 = meshSection.ProcVertexBuffer.Add(vertex2);
		int32 index1 = meshSection.ProcVertexBuffer.Add(vertex1);
		int32 index0 = meshSection.ProcVertexBuffer.Add(vertex0);

		// Add to index buffer
		// Again, this is done in reverse
		meshSection.ProcIndexBuffer.Add(index2);
		meshSection.ProcIndexBuffer.Add(index1);
		meshSection.ProcIndexBuffer.Add(index0);
	}

	meshSection.bEnableCollision = bShouldEnableCollision;
	return meshSection;
}

FVoxelMesh UVoxelProceduralMeshComponent::GetEncodedMesh(ABaseVolume* Volume, FRegion Region, TSubclassOf<UMarchingCubesDefaultController> Controller)
{
	// Validate parameters
	checkf(Volume != NULL, TEXT("Provided volume cannot be null"));
	checkf(Controller != NULL, TEXT("Controller type must be provided."));
	FVoxelMesh result;

	UMarchingCubesDefaultController* controller = NewObject<UMarchingCubesDefaultController>((UObject*)GetTransientPackage(),Controller);

	// Store some commonly used values for performance and convienience
	const uint32 uRegionWidthInVoxels = (uint32)URegionHelper::GetWidthInVoxels(Region);
	const uint32 uRegionHeightInVoxels = (uint32)URegionHelper::GetHeightInVoxels(Region);
	const uint32 uRegionDepthInVoxels = (uint32)URegionHelper::GetDepthInVoxels(Region);

	auto Threshold = controller->GetThreshold();

	// A naive implementation of Marching Cubes might sample the eight corner voxels of every cell to determine the cell index. 
	// However, when processing the cells sequentially we can observe that many of the voxels are shared with previous adjacent 
	// cells, and so we can obtain these by careful bit-shifting. These variables keep track of previous cells for this purpose.
	// We don't clear the arrays because the algorithm ensures that we only read from elements we have previously written to.
	uint8 uPreviousCellIndex = 0;
	TArray<uint8> pPreviousRowCellIndices;
	pPreviousRowCellIndices.SetNumUninitialized(uRegionWidthInVoxels);
	TArray<uint8> pPreviousSliceCellIndices;
	pPreviousSliceCellIndices.AddUninitialized(uRegionWidthInVoxels * uRegionHeightInVoxels);

	// A given vertex may be shared by multiple triangles, so we need to keep track of the indices into the vertex array.
	// We don't clear the arrays because the algorithm ensures that we only read from elements we have previously written to.
	TArray<FVector> pIndices;
	pIndices.SetNumUninitialized(uRegionWidthInVoxels * uRegionHeightInVoxels);
	TArray<FVector> pPreviousIndices;
	pPreviousIndices.SetNumUninitialized(uRegionWidthInVoxels * uRegionHeightInVoxels);

	// A sampler pointing at the beginning of the region, which gets incremented to always point at the beginning of a slice.

	UVolumeSampler* startOfSlice = NewObject<UVolumeSampler>();
	startOfSlice->Initalize(Volume);
	startOfSlice->SetPosition(URegionHelper::GetLowerX(Region), URegionHelper::GetLowerY(Region), URegionHelper::GetLowerZ(Region));

	for (uint32 uZRegSpace = 0; uZRegSpace < uRegionDepthInVoxels; uZRegSpace++)
	{
		// A sampler pointing at the beginning of the slice, which gets incremented to always point at the beginning of a row.
		UVolumeSampler* startOfRow = NewObject<UVolumeSampler>((UObject*)GetTransientPackage(), NAME_None, RF_NoFlags, startOfSlice);

		for (uint32 uYRegSpace = 0; uYRegSpace < uRegionHeightInVoxels; uYRegSpace++)
		{
			// Copying a sampler which is already pointing at the correct location seems (slightly) faster than
			// calling setPosition(). Therefore we make use of 'startOfRow' and 'startOfSlice' to reset the sampler.
			UVolumeSampler* sampler = NewObject<UVolumeSampler>((UObject*)GetTransientPackage(), NAME_None, RF_NoFlags, startOfRow);

			for (uint32 uXRegSpace = 0; uXRegSpace < uRegionWidthInVoxels; uXRegSpace++)
			{
				// Note: In many cases the provided region will be (mostly) empty which means mesh vertices/indices 
				// are not generated and the only thing that is done for each cell is the computation of uCellIndex.
				// It appears that retrieving the voxel value is not so expensive and that it is the bitwise combining
				// which actually carries the cost.
				//
				// If we really need to speed this up more then it may be possible to pack 4 8-bit cell indices into
				// a single 32-bit value and then perform the bitwise logic on all four of them at the same time. 
				// However, this complicates the code and there would still be the cost of packing/unpacking so it's
				// not clear if there is really a benefit. It's something to consider in the future.

				// Each bit of the cell index specifies whether a given corner of the cell is above or below the threshold.
				uint8 uCellIndex = 0;

				// Four bits of our cube index are obtained by looking at the cube index for
				// the previous slice and copying four of those bits into their new positions.
				uint8 uPreviousCellIndexZ = UArrayHelper::Get2DUint8(pPreviousSliceCellIndices, uXRegSpace, uYRegSpace, uRegionWidthInVoxels);
				uPreviousCellIndexZ >>= 4;
				uCellIndex |= uPreviousCellIndexZ;

				// Two bits of our cube index are obtained by looking at the cube index for
				// the previous row and copying two of those bits into their new positions.
				uint8 uPreviousCellIndexY = pPreviousRowCellIndices[uXRegSpace];
				uPreviousCellIndexY &= 204; //204 = 128+64+8+4
				uPreviousCellIndexY >>= 2;
				uCellIndex |= uPreviousCellIndexY;

				// One bit of our cube index are obtained by looking at the cube index for
				// the previous cell and copying one of those bits into it's new position.
				uint8 UPreviousCellIndexX = uPreviousCellIndex;
				UPreviousCellIndexX &= 170; //170 = 128+32+8+2
				UPreviousCellIndexX >>= 1;
				uCellIndex |= UPreviousCellIndexX;

				// The last bit of our cube index is obtained by looking
				// at the relevant voxel and comparing it to the threshold
				UVoxel* v111 = sampler->GetVoxel();
				if (controller->ConvertToDensity(v111) < Threshold) uCellIndex |= 128;

				// The current value becomes the previous value, ready for the next iteration.
				uPreviousCellIndex = uCellIndex;
				pPreviousRowCellIndices[uXRegSpace] = uCellIndex;
				pPreviousSliceCellIndices = UArrayHelper::Set2DUint8(pPreviousSliceCellIndices, uCellIndex, uXRegSpace, uYRegSpace, uRegionWidthInVoxels);

				// 12 bits of uEdge determine whether a vertex is placed on each of the 12 edges of the cell.
				uint16 uEdge = EdgeTable[uCellIndex];

				// Test whether any vertices and indices should be generated for the current cell (i.e. it is occupied).
				// Performance note: This condition is usually false because most cells in a volume are completely above
				// or below the threshold and hence unoccupied. However, even when it is always false (testing on an empty
				// volume) it still incurs significant overhead, probably because the code is large and bloats the for loop
				// which contains it. On my empty volume test case the code as given runs in 34ms, but if I replace the
				// condition with 'false' it runs in 24ms and gives the same output (i.e. none).
				//
				// An improvement is to move the code into a separate function which does speed things up (30ms), but this
				// is messy as the function needs to be passed about 10 different parameters, probably adding some overhead 
				// in its self. This does indeed seem to slow down the case when cells are occupied, by about 10-20%.
				//
				// Overall I don't know the right solution, but I'm leaving the code as-is to avoid making it messy. If we
				// can reduce the number of parameters which need to be passed then it might be worth moving it into a
				// function, or otherwise it may simply be worth trying to shorten the code (e.g. adding other function
				// calls). For now we will leave it as-is, until we have more information from real-world profiling.
				if (uEdge != 0)
				{
					auto v111Density = controller->ConvertToDensity(v111);

					// Performance note: Computing normals is one of the bottlencks in the mesh generation process. The
					// central difference approach actually samples the same voxel more than once as we call it on two
					// adjacent voxels. Perhaps we could expand this and eliminate dupicates in the future. Alternatively, 
					// we could compute vertex normals from adjacent face normals instead of via central differencing, 
					// but not for vertices on the edge of the region (as this causes visual discontinities).
					//const FVector n111 = computeCentralDifferenceGradient(sampler, controller);

					/* Find the vertices where the surface intersects the cube */
					if ((uEdge & 64) && (uXRegSpace > 0))
					{
						sampler->MoveNegativeX();
						UVoxel* v011 = sampler->GetVoxel();
						auto v011Density = controller->ConvertToDensity(v011);
						const float fInterp = static_cast<float>(Threshold - v011Density) / static_cast<float>(v111Density - v011Density);

						// Compute the position
						const FVector v3dPosition(static_cast<float>(uXRegSpace - 1) + fInterp, static_cast<float>(uYRegSpace), static_cast<float>(uZRegSpace));

						// Compute the normal
						//const FVector n011 = computeCentralDifferenceGradient(sampler, controller);
						//FVector v3dNormal = (n111*fInterp) + (n011*(1 - fInterp));

						// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
						// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
						//if (v3dNormal.lengthSquared() > 0.000001f)
						//{
							//v3dNormal.normalise();
						//}

						// Allow the controller to decide how the material should be derived from the voxels.
						UVoxel* uMaterial = controller->BlendMaterials(v011, v111, fInterp);

						FVoxelVertex surfaceVertex;
						const FVector v3dScaledPosition(static_cast<uint16>(v3dPosition.X * 256.0f), static_cast<uint16>(v3dPosition.Y * 256.0f), static_cast<uint16>(v3dPosition.Z * 256.0f));
						surfaceVertex.Position = v3dScaledPosition;
						//surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
						surfaceVertex.Data = uMaterial;

						result = AddVertex(result, surfaceVertex);
						const uint32 uLastVertexIndex = result.Vertices.Num() - 1;
						FVector pIndex = UArrayHelper::Get2DFVector(pIndices, uXRegSpace, uYRegSpace, uRegionWidthInVoxels);
						pIndex.X = uLastVertexIndex;
						pIndices = UArrayHelper::Set2DFVector(pIndices, pIndex, uXRegSpace, uYRegSpace, uRegionWidthInVoxels);

						sampler->MovePositiveX();
					}
					if ((uEdge & 32) && (uYRegSpace > 0))
					{
						sampler->MoveNegativeY();
						UVoxel* v101 = sampler->GetVoxel();
						auto v101Density = controller->ConvertToDensity(v101);
						const float fInterp = static_cast<float>(Threshold - v101Density) / static_cast<float>(v111Density - v101Density);

						// Compute the position
						const FVector v3dPosition(static_cast<float>(uXRegSpace), static_cast<float>(uYRegSpace - 1) + fInterp, static_cast<float>(uZRegSpace));

						// Compute the normal
						//const FVector n101 = computeCentralDifferenceGradient(sampler, controller);
						//FVector v3dNormal = (n111*fInterp) + (n101*(1 - fInterp));

						// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
						// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
						//if (v3dNormal.lengthSquared() > 0.000001f)
						//{
							//v3dNormal.normalise();
						//}

						// Allow the controller to decide how the material should be derived from the voxels.
						UVoxel* uMaterial = controller->BlendMaterials(v101, v111, fInterp);

						FVoxelVertex surfaceVertex;
						const FVector v3dScaledPosition(static_cast<uint16_t>(v3dPosition.X * 256.0f), static_cast<uint16_t>(v3dPosition.Y * 256.0f), static_cast<uint16_t>(v3dPosition.Z * 256.0f));
						surfaceVertex.Position = v3dScaledPosition;
						//surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
						surfaceVertex.Data = uMaterial;

						result = AddVertex(result, surfaceVertex);
						const uint32 uLastVertexIndex = result.Vertices.Num() - 1;
						FVector pIndex = UArrayHelper::Get2DFVector(pIndices, uXRegSpace, uYRegSpace, uRegionWidthInVoxels);
						pIndex.Y = uLastVertexIndex;
						pIndices = UArrayHelper::Set2DFVector(pIndices, pIndex, uXRegSpace, uYRegSpace, uRegionWidthInVoxels);

						sampler->MovePositiveY();
					}
					if ((uEdge & 1024) && (uZRegSpace > 0))
					{
						sampler->MoveNegativeZ();
						UVoxel* v110 = sampler->GetVoxel();
						auto v110Density = controller->ConvertToDensity(v110);
						const float fInterp = static_cast<float>(Threshold - v110Density) / static_cast<float>(v111Density - v110Density);

						// Compute the position
						const FVector v3dPosition(static_cast<float>(uXRegSpace), static_cast<float>(uYRegSpace), static_cast<float>(uZRegSpace - 1) + fInterp);

						// Compute the normal
						//const FVector n110 = computeCentralDifferenceGradient(sampler, controller);
						//FVector v3dNormal = (n111*fInterp) + (n110*(1 - fInterp));

						// The gradient for a voxel can be zero (e.g. solid voxel surrounded by empty ones) and so
						// the interpolated normal can also be zero (e.g. a grid of alternating solid and empty voxels).
						//if (v3dNormal.lengthSquared() > 0.000001f)
						//{
							//v3dNormal.normalise();
						//}

						// Allow the controller to decide how the material should be derived from the voxels.
						UVoxel* uMaterial = controller->BlendMaterials(v110, v111, fInterp);

						FVoxelVertex surfaceVertex;
						const FVector v3dScaledPosition(static_cast<uint16_t>(v3dPosition.X * 256.0f), static_cast<uint16_t>(v3dPosition.Y * 256.0f), static_cast<uint16_t>(v3dPosition.Z * 256.0f));
						surfaceVertex.Position = v3dScaledPosition;
						//surfaceVertex.encodedNormal = encodeNormal(v3dNormal);
						surfaceVertex.Data = uMaterial;

						result = AddVertex(result, surfaceVertex);
						const uint32 uLastVertexIndex = result.Vertices.Num() - 1;
						FVector pIndex = UArrayHelper::Get2DFVector(pIndices, uXRegSpace, uYRegSpace, uRegionWidthInVoxels);
						pIndex.Z = uLastVertexIndex;
						pIndices = UArrayHelper::Set2DFVector(pIndices, pIndex, uXRegSpace, uYRegSpace, uRegionWidthInVoxels);

						sampler->MovePositiveZ();
					}

					// Now output the indices. For the first row, column or slice there aren't
					// any (the region size in cells is one less than the region size in voxels)
					if ((uXRegSpace != 0) && (uYRegSpace != 0) && (uZRegSpace != 0))
					{

						int32 indlist[12];

						/* Find the vertices where the surface intersects the cube */
						if (uEdge & 1)
						{
							indlist[0] = UArrayHelper::Get2DFVector(pPreviousIndices, uXRegSpace, uYRegSpace - 1, uRegionWidthInVoxels).X;
						}
						if (uEdge & 2)
						{
							indlist[1] = UArrayHelper::Get2DFVector(pPreviousIndices, uXRegSpace, uYRegSpace, uRegionWidthInVoxels).Y;
						}
						if (uEdge & 4)
						{
							indlist[2] = UArrayHelper::Get2DFVector(pPreviousIndices, uXRegSpace, uYRegSpace, uRegionWidthInVoxels).X;
						}
						if (uEdge & 8)
						{
							indlist[3] = UArrayHelper::Get2DFVector(pPreviousIndices, uXRegSpace - 1, uYRegSpace, uRegionWidthInVoxels).Y;
						}
						if (uEdge & 16)
						{
							indlist[4] = UArrayHelper::Get2DFVector(pIndices, uXRegSpace, uYRegSpace - 1, uRegionWidthInVoxels).X;
						}
						if (uEdge & 32)
						{
							indlist[5] = UArrayHelper::Get2DFVector(pIndices, uXRegSpace, uYRegSpace, uRegionWidthInVoxels).Y;
						}
						if (uEdge & 64)
						{
							indlist[6] = UArrayHelper::Get2DFVector(pIndices, uXRegSpace, uYRegSpace, uRegionWidthInVoxels).X;
						}
						if (uEdge & 128)
						{
							indlist[7] = UArrayHelper::Get2DFVector(pIndices, uXRegSpace - 1, uYRegSpace, uRegionWidthInVoxels).Y;
						}
						if (uEdge & 256)
						{
							indlist[8] = UArrayHelper::Get2DFVector(pIndices, uXRegSpace - 1, uYRegSpace - 1, uRegionWidthInVoxels).Z;
						}
						if (uEdge & 512)
						{
							indlist[9] = UArrayHelper::Get2DFVector(pIndices, uXRegSpace, uYRegSpace - 1, uRegionWidthInVoxels).Z;
						}
						if (uEdge & 1024)
						{
							indlist[10] = UArrayHelper::Get2DFVector(pIndices, uXRegSpace, uYRegSpace, uRegionWidthInVoxels).Z;
						}
						if (uEdge & 2048)
						{
							indlist[11] = UArrayHelper::Get2DFVector(pIndices, uXRegSpace - 1, uYRegSpace, uRegionWidthInVoxels).Z;
						}

						for (int i = 0; TriTable[uCellIndex][i] != -1; i += 3)
						{
							const int32 ind0 = indlist[TriTable[uCellIndex][i]];
							const int32 ind1 = indlist[TriTable[uCellIndex][i + 1]];
							const int32 ind2 = indlist[TriTable[uCellIndex][i + 2]];

							if ((ind0 != -1) && (ind1 != -1) && (ind2 != -1))
							{
								result = AddTriangle(result, ind0, ind1, ind2);
							}
						} // For each triangle
					}
				} // For each cell
				sampler->MovePositiveX();
			} // For X
			startOfRow->MovePositiveY();
		} // For Y
		startOfSlice->MovePositiveZ();

		TArray<FVector> tem = pIndices;
		pIndices = pPreviousIndices;
		pPreviousIndices = tem;
	} // For Z

	result.Offset = URegionHelper::GetLowerCorner(Region);
	return result;
}

FVoxelMesh UVoxelProceduralMeshComponent::GetDecodedMesh(FVoxelMesh EncodedMesh)
{
	FVoxelMesh decodedMesh;

	for (int32 ct = 0; ct < EncodedMesh.Vertices.Num(); ct++)
	{
		FVoxelVertex decodedVertex;
		FVector encodedPosition = EncodedMesh.Vertices[ct].Position;
		FVector result(encodedPosition.X, encodedPosition.Y, encodedPosition.Z);
		result *= (1.0f / 256.0f); // Division is compile-time constant
		
		decodedVertex.Position = result;
		decodedVertex.Data = EncodedMesh.Vertices[ct].Data;
		
		decodedMesh = AddVertex(decodedMesh, decodedVertex);
	}

	checkf(EncodedMesh.Indices.Num() % 3 == 0, TEXT("The number of indices must always be a multiple of three."));
	for (int32 ct = 0; ct < EncodedMesh.Indices.Num(); ct += 3)
	{
		decodedMesh = AddTriangle(decodedMesh, EncodedMesh.Indices[ct], EncodedMesh.Indices[ct + 1], EncodedMesh.Indices[ct + 2]);
	}

	decodedMesh.Offset = EncodedMesh.Offset;

	return decodedMesh;
}

TArray<FVoxelMeshSection> UVoxelProceduralMeshComponent::GenerateTriangles(const FVoxelMesh& ExtractedMesh)
{
	TArray<FVoxelMeshSection> meshSections;
	auto decodedMesh = GetDecodedMesh(ExtractedMesh);

	// Sanity check
	if (decodedMesh.Indices.Num() < 3)
	{
		UE_LOG(LogPolyVox, Error, TEXT("Not enough vertices to create any procedural meshes!"));
		return meshSections;
	}

	for (int i = 0; i < decodedMesh.Indices.Num() - 2; i += 3)
	{
		// Create triangle
		FVoxelTriangle triangle;

		// Get Vertex 0
		auto index = decodedMesh.Indices[i];
		FVoxelVertex vertex = decodedMesh.Vertices[index];
		triangle.Vertex0 = vertex;
		triangle.Vertex0.Position += decodedMesh.Offset;

		// Get Vertex 1
		index = decodedMesh.Indices[i + 1];
		vertex = decodedMesh.Vertices[index];
		triangle.Vertex1 = vertex;
		triangle.Vertex1.Position += decodedMesh.Offset;

		// Get Vertex 2
		index = decodedMesh.Indices[i + 2];
		vertex = decodedMesh.Vertices[index];
		triangle.Vertex2 = vertex;
		triangle.Vertex2.Position += decodedMesh.Offset;

		// Get Material ID
		uint8 materialID = triangle.Vertex0.Data->Material;
		if (triangle.Vertex1.Data->Material == triangle.Vertex2.Data->Material)
		{
			materialID = triangle.Vertex1.Data->Material;
		}
		triangle.MaterialID = materialID;
		if (materialID >= meshSections.Num())
		{
			meshSections.SetNumZeroed(materialID + 1);
		}

		// Add to output
		meshSections[materialID].Triangles.Add(triangle);
	}
	return meshSections;
}