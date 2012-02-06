
// warning at 949 code page
#pragma warning( disable : 4819 ) 

#include <cuda_runtime.h>


#include "cuKDTree.h"
#include "cuKDTreeNode.h"
#include "../GIEngineCore/Global.h"

// 32 * 32 = 1024 <= 1024 (GTS 450)
const int BLOCK_SIZE_X = 32;
const int BLOCK_SIZE_Y = 32;

__global__ void ShootRay( GIHit *outHits, RtTriAccel *TriAccelArray, cuKDTree *KDTree, const GIRay *RayArray )
{
	int bx = blockIdx.x;
	int by = blockIdx.y;
	int tx = threadIdx.x;
	int ty = threadIdx.y;

	int WIDTH = BLOCK_SIZE_X * gridDim.x;

	int index = tx + bx * BLOCK_SIZE_X
		+ (ty + by * BLOCK_SIZE_Y) * WIDTH;
	
	const GIRay &Ray = RayArray[index];
	GIHit &result = outHits[index];

	result.triNum = 1;
	result.dist = 3.141592f;
}

extern "C" __host__ void cuShootRays( cuKDTree *DeviceKDTree, unsigned int RayCount, const GIRay *RayArray, GIHit *outHits )
{
	GIRay *DeviceRayArray;
	cudaMalloc( &DeviceRayArray, RayCount * sizeof(GIRay) );
	cudaMemcpy( DeviceRayArray, RayArray, RayCount * sizeof(GIRay), cudaMemcpyHostToDevice );

	GIHit *DeviceHitArray;
	cudaMalloc( &DeviceHitArray, RayCount * sizeof(GIHit) );

	//cudaMemcpy( RayArray, DeviceRayArray, RayCount * sizeof(cuKDTreeNode), cudaMemcpyHostToDevice );
	

	// TODO: 크기 비교 엄밀히!! 제대로 이해하고!!
	dim3 dimGrid( RayCount/BLOCK_SIZE_X, 1, 1 );
	dim3 dimBlock( BLOCK_SIZE_X, 1, 1 );

	ShootRay<<<dimGrid, dimBlock>>>( DeviceHitArray, NULL, DeviceKDTree, DeviceRayArray );
	
	cudaMemcpy( outHits, DeviceHitArray, RayCount * sizeof(GIHit), cudaMemcpyDeviceToHost );

	cudaFree( DeviceRayArray );
	cudaFree( DeviceHitArray );
}

