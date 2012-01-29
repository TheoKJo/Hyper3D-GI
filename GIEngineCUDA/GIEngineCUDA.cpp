/*!
 * \file		GIEngineCUDA.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/01/22
 * created:		2012/01/22 13:17
 * copyright	Hybrid (creavol@gmail.com)
 */

#include "GIEngineCUDA_PCH.h"

//#include "../GIEngine/GIEngine.h"
#include "../GIEngine/Raytracer/Raytracer.h"
#include "GIEngineCUDA.h"
#include "KDTreeGPU.h"

using namespace GIEngine;

void ShootRays( GIHit *outHits, RtTriAccel *TriAccelArray, KDTreeGPU *KDTree, const GIRay *RayArray, unsigned int RayCount );


void CheckPerformance()
{
	// m_GlobalMemorySize;
	// m_ConstantMemorySize;
	// m_SharedMemorySizePerBlock;
	// 

	// max thread per sm 은 어떻게 알지??

	// TODO: Hardware의 한계와 현재 설정을 변수 명에서 구분해야할듯.

	unsigned int m_SMCount;
	unsigned int m_SPCountPerSM;
	unsigned int m_CUDA_Cores;
	unsigned int m_WarpSize;
	unsigned int m_MaxThreadsPerBlock;
	unsigned int m_MaxThreadsPerSM;
	//CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR

	unsigned int GridSizeX;
	unsigned int GridSizeY;
	unsigned int GridSizeZ;
	//! number of blocks per grid
	unsigned int BlocksPerGrid = GridSizeX * GridSizeY * GridSizeZ;
	
	unsigned int BlockSizeX;
	unsigned int BlockSizeY;
	unsigned int BlockSizeZ;
	//! number of threads per block
	unsigned int ThreadsPerBlock = BlockSizeX * BlockSizeY * BlockSizeZ;

	// number of warps per block
	unsigned int WarpsPerBlock = ThreadsPerBlock/m_WarpSize;

	// TODO: spec?
	unsigned int MaxBlocksPerSM = 8;


	if( WarpsPerBlock * MaxBlocksPerSM < m_SPCountPerSM )
	{
		// WarpsPerBlock * MaxBlocksPerSM: SM에 할당 된 warps
		// SM에 할당된 warps가 너무 적음. SPs per SMs 보다 많기를 권장?
	}

	unsigned int BlocksPerSM = m_MaxThreadsPerSM/ThreadsPerBlock;
	BlocksPerSM = BlocksPerSM>MaxBlocksPerSM?MaxBlocksPerSM:BlocksPerSM;
	BlocksPerSM
}

KDTreeGPU* Raytracer::CreateKDTreeGPU( RtScene *rtScene )
{
	// TODO: type check
	KDTreeStructure *KDTreeCPU = rtScene->GetAccStructure();
	int NodeCount = KDTreeCPU->GetNodeSize();
	KDTreeGPU kdtree = new KDTreeGPU;
	//kdtree->
}

void Raytracer::ShootRays( GIHit *outHits, RtScene *rtScene, const GIRay *RayArray, unsigned int RayCount )
{
	// TODO: 삼각형 포함, RtScene 도 바꿔야함! KDTree 그대로 당연히 못씀.
	ShootRays( outHits, rtScene->GetTriAccelList(), (KDTreeGPU*)rtScene->GetAccStructure(), RayArray, RayCount );
}