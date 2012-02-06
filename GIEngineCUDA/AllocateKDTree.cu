
#include "cuKDTree.h"
#include "cuKDTreeNode.h"

extern "C" __host__ cuKDTree* CreateDeviceKDTreeAllocateKDTreeMemory( unsigned int NodeCount, cuKDTreeNode *NodeArray, 
															  unsigned int TriangleCount, cuKDTreeTriangle *TriangleArray, 
															  unsigned int TriangleIndexCount, unsigned int *TriangleIndexArray )
{
	cuKDTree *DeviceKDTree = new cuKDTree();

	DeviceKDTree->NodeCount = NodeCount;
	DeviceKDTree->TriangleCount = TriangleCount;
	DeviceKDTree->TriangleIndexCount = TriangleIndexCount;

	// TODO: Error Check

	cudaError_t result;

	// node array
	DeviceKDTree->NodeArray = NULL;
	result = cudaMalloc( &DeviceKDTree->NodeArray, NodeCount * sizeof(cuKDTreeNode) );
	if( result != cudaSuccess )
	{
		int a = result;
		return NULL;
	}
	result = cudaMemcpy( DeviceKDTree->NodeArray, NodeArray, NodeCount * sizeof(cuKDTreeNode), cudaMemcpyHostToDevice );
	if( result != cudaSuccess )
	{
		int a = result;
		return NULL;
	}

	// triangles
	DeviceKDTree->TriangleArray = NULL;
	result = cudaMalloc( &DeviceKDTree->TriangleArray, TriangleCount * sizeof(cuKDTreeTriangle) );
	if( result != cudaSuccess )
	{
		int a = result;
		return NULL;
	}
	result = cudaMemcpy( DeviceKDTree->TriangleArray, TriangleArray, TriangleCount * sizeof(cuKDTreeTriangle), cudaMemcpyHostToDevice );
	if( result != cudaSuccess )
	{
		int a = result;
		return NULL;
	}

	// triangle indices
	DeviceKDTree->TriangleIndexArray = NULL;
	result = cudaMalloc( &DeviceKDTree->TriangleIndexArray, TriangleIndexCount * sizeof(unsigned int) );
	if( result != cudaSuccess )
	{
		int a = result;
		return NULL;
	}
	result = cudaMemcpy( DeviceKDTree->TriangleIndexArray, TriangleIndexArray, TriangleIndexCount * sizeof(unsigned int), cudaMemcpyHostToDevice );
	if( result != cudaSuccess )
	{
		int a = result;
		return NULL;
	}


	return DeviceKDTree;
}