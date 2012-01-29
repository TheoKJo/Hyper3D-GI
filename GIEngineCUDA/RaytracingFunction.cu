
// warning at 949 code page
#pragma warning( disable : 4819 ) 

#include <cuda_runtime.h>
#include "../GIEngineCore/Global.h"

// 32 * 32 = 1024 <= 1024 (GTS 450)
const int BLOCK_SIZE_X = 32;
const int BLOCK_SIZE_Y = 32;

class KDTreeGPU;

__global__ void ShootRayCUDA( GIHit *outHits, RtTriAccel *TriAccelArray, KDTreeGPU *KDTree, const GIRay *RayArray )
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
}

__host__ void ShootRays( GIHit *outHits, RtTriAccel *TriAccelArray, KDTreeGPU *KDTree, const GIRay *RayArray, unsigned int RayCount )
{
	// TODO: 크기 비교 엄밀히!! 제대로 이해하고!!
	dim3 dimGrid( RayCount/BLOCK_SIZE_X, 1, 1 );
	dim3 dimBlock( BLOCK_SIZE_X, 1, 1 );

	ShootRayCUDA<<<dimGrid, dimBlock>>>( outHits, TriAccelArray, KDTree, RayArray );
}