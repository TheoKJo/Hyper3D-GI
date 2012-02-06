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
#include <Scene.h>

#include "GIEngineCUDA.h"
#include "../GIEngine/Raytracer/KDTreeStructure.h"
#include "KDTreeStructureCUDA.h"
#include "cuKDTreeNode.h"

#include <stack>

// CUDA
extern "C" cuKDTree* CreateDeviceKDTreeAllocateKDTreeMemory( unsigned int NodeCount, cuKDTreeNode *NodeArray, 
															  unsigned int TriangleCount, cuKDTreeTriangle *TriangleArray, 
															  unsigned int TriangleIndexCount, unsigned int *TriangleIndexArray );

extern "C" void cuShootRays( cuKDTree *DeviceKDTree, unsigned int RayCount, const GIRay *RayArray, GIHit *outHits );
//void cuShootRays( GIHit *outHits, GITriAccel *TriAccelArray, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, const GIRay *RayArray );

bool InitializeCUDA()
{
	return true;
}

void CheckPerformance()
{
	// m_GlobalMemorySize;
	// m_ConstantMemorySize;
	// m_SharedMemorySizePerBlock;
	// 

	// max thread per sm 은 어떻게 알지??

	// TODO: Hardware의 한계와 현재 설정을 변수 명에서 구분해야할듯.

	//unsigned int m_SMCount;
	//unsigned int m_SPCountPerSM;
	//unsigned int m_CUDA_Cores;
	//unsigned int m_WarpSize;
	//unsigned int m_MaxThreadsPerBlock;
	//unsigned int m_MaxThreadsPerSM;
	////CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR

	//unsigned int GridSizeX;
	//unsigned int GridSizeY;
	//unsigned int GridSizeZ;
	////! number of blocks per grid
	//unsigned int BlocksPerGrid = GridSizeX * GridSizeY * GridSizeZ;
	//
	//unsigned int BlockSizeX;
	//unsigned int BlockSizeY;
	//unsigned int BlockSizeZ;
	////! number of threads per block
	//unsigned int ThreadsPerBlock = BlockSizeX * BlockSizeY * BlockSizeZ;

	//// number of warps per block
	//unsigned int WarpsPerBlock = ThreadsPerBlock/m_WarpSize;

	//// TODO: spec?
	//unsigned int MaxBlocksPerSM = 8;


	//if( WarpsPerBlock * MaxBlocksPerSM < m_SPCountPerSM )
	//{
	//	// WarpsPerBlock * MaxBlocksPerSM: SM에 할당 된 warps
	//	// SM에 할당된 warps가 너무 적음. SPs per SMs 보다 많기를 권장?
	//}

	//unsigned int BlocksPerSM = m_MaxThreadsPerSM/ThreadsPerBlock;
	//BlocksPerSM = BlocksPerSM>MaxBlocksPerSM?MaxBlocksPerSM:BlocksPerSM;
	////BlocksPerSM
}
//
//void ErrorStringFromCURAError( char* outString, cudaError errorCode )
//{
//	/**
//	* The API call returned with no errors. In the case of query calls, this
//	* can also mean that the operation being queried is complete (see
//	* ::cudaEventQuery() and ::cudaStreamQuery()).
//	*/
//	cudaSuccess                           =      0,
//
//	/**
//	* The device function being invoked (usually via ::cudaLaunch()) was not
//	* previously configured via the ::cudaConfigureCall() function.
//	*/
//	cudaErrorMissingConfiguration         =      1,
//
//	/**
//	* The API call failed because it was unable to allocate enough memory to
//	* perform the requested operation.
//	*/
//	cudaErrorMemoryAllocation             =      2,
//
//	/**
//	* The API call failed because the CUDA driver and runtime could not be
//	* initialized.
//	*/
//	cudaErrorInitializationError          =      3,
//
//	/**
//	* An exception occurred on the device while executing a kernel. Common
//	* causes include dereferencing an invalid device pointer and accessing
//	* out of bounds shared memory. The device cannot be used until
//	* ::cudaThreadExit() is called. All existing device memory allocations
//	* are invalid and must be reconstructed if the program is to continue
//	* using CUDA.
//	*/
//	cudaErrorLaunchFailure                =      4,
//
//	/**
//	* This indicated that a previous kernel launch failed. This was previously
//	* used for device emulation of kernel launches.
//	* \deprecated
//	* This error return is deprecated as of CUDA 3.1. Device emulation mode was
//	* removed with the CUDA 3.1 release.
//	*/
//	cudaErrorPriorLaunchFailure           =      5,
//
//	/**
//	* This indicates that the device kernel took too long to execute. This can
//	* only occur if timeouts are enabled - see the device property
//	* \ref ::cudaDeviceProp::kernelExecTimeoutEnabled "kernelExecTimeoutEnabled"
//	* for more information. The device cannot be used until ::cudaThreadExit()
//	* is called. All existing device memory allocations are invalid and must be
//	* reconstructed if the program is to continue using CUDA.
//	*/
//	cudaErrorLaunchTimeout                =      6,
//
//	/**
//	* This indicates that a launch did not occur because it did not have
//	* appropriate resources. Although this error is similar to
//	* ::cudaErrorInvalidConfiguration, this error usually indicates that the
//	* user has attempted to pass too many arguments to the device kernel, or the
//	* kernel launch specifies too many threads for the kernel's register count.
//	*/
//	cudaErrorLaunchOutOfResources         =      7,
//
//	/**
//	* The requested device function does not exist or is not compiled for the
//	* proper device architecture.
//	*/
//	cudaErrorInvalidDeviceFunction        =      8,
//
//	/**
//	* This indicates that a kernel launch is requesting resources that can
//	* never be satisfied by the current device. Requesting more shared memory
//	* per block than the device supports will trigger this error, as will
//	* requesting too many threads or blocks. See ::cudaDeviceProp for more
//	* device limitations.
//	*/
//	cudaErrorInvalidConfiguration         =      9,
//
//	/**
//	* This indicates that the device ordinal supplied by the user does not
//	* correspond to a valid CUDA device.
//	*/
//	cudaErrorInvalidDevice                =     10,
//
//	/**
//	* This indicates that one or more of the parameters passed to the API call
//	* is not within an acceptable range of values.
//	*/
//	cudaErrorInvalidValue                 =     11,
//
//	/**
//	* This indicates that one or more of the pitch-related parameters passed
//	* to the API call is not within the acceptable range for pitch.
//	*/
//	cudaErrorInvalidPitchValue            =     12,
//
//	/**
//	* This indicates that the symbol name/identifier passed to the API call
//	* is not a valid name or identifier.
//	*/
//	cudaErrorInvalidSymbol                =     13,
//
//	/**
//	* This indicates that the buffer object could not be mapped.
//	*/
//	cudaErrorMapBufferObjectFailed        =     14,
//
//	/**
//	* This indicates that the buffer object could not be unmapped.
//	*/
//	cudaErrorUnmapBufferObjectFailed      =     15,
//
//	/**
//	* This indicates that at least one host pointer passed to the API call is
//	* not a valid host pointer.
//	*/
//	cudaErrorInvalidHostPointer           =     16,
//
//	/**
//	* This indicates that at least one device pointer passed to the API call is
//	* not a valid device pointer.
//	*/
//	cudaErrorInvalidDevicePointer         =     17,
//
//	/**
//	* This indicates that the texture passed to the API call is not a valid
//	* texture.
//	*/
//	cudaErrorInvalidTexture               =     18,
//
//	/**
//	* This indicates that the texture binding is not valid. This occurs if you
//	* call ::cudaGetTextureAlignmentOffset() with an unbound texture.
//	*/
//	cudaErrorInvalidTextureBinding        =     19,
//
//	/**
//	* This indicates that the channel descriptor passed to the API call is not
//	* valid. This occurs if the format is not one of the formats specified by
//	* ::cudaChannelFormatKind, or if one of the dimensions is invalid.
//	*/
//	cudaErrorInvalidChannelDescriptor     =     20,
//
//	/**
//	* This indicates that the direction of the memcpy passed to the API call is
//	* not one of the types specified by ::cudaMemcpyKind.
//	*/
//	cudaErrorInvalidMemcpyDirection       =     21,
//
//	/**
//	* This indicated that the user has taken the address of a constant variable,
//	* which was forbidden up until the CUDA 3.1 release.
//	* \deprecated
//	* This error return is deprecated as of CUDA 3.1. Variables in constant
//	* memory may now have their address taken by the runtime via
//	* ::cudaGetSymbolAddress().
//	*/
//	cudaErrorAddressOfConstant            =     22,
//
//	/**
//	* This indicated that a texture fetch was not able to be performed.
//	* This was previously used for device emulation of texture operations.
//	* \deprecated
//	* This error return is deprecated as of CUDA 3.1. Device emulation mode was
//	* removed with the CUDA 3.1 release.
//	*/
//	cudaErrorTextureFetchFailed           =     23,
//
//	/**
//	* This indicated that a texture was not bound for access.
//	* This was previously used for device emulation of texture operations.
//	* \deprecated
//	* This error return is deprecated as of CUDA 3.1. Device emulation mode was
//	* removed with the CUDA 3.1 release.
//	*/
//	cudaErrorTextureNotBound              =     24,
//
//	/**
//	* This indicated that a synchronization operation had failed.
//	* This was previously used for some device emulation functions.
//	* \deprecated
//	* This error return is deprecated as of CUDA 3.1. Device emulation mode was
//	* removed with the CUDA 3.1 release.
//	*/
//	cudaErrorSynchronizationError         =     25,
//
//	/**
//	* This indicates that a non-float texture was being accessed with linear
//	* filtering. This is not supported by CUDA.
//	*/
//	cudaErrorInvalidFilterSetting         =     26,
//
//	/**
//	* This indicates that an attempt was made to read a non-float texture as a
//	* normalized float. This is not supported by CUDA.
//	*/
//	cudaErrorInvalidNormSetting           =     27,
//
//	/**
//	* Mixing of device and device emulation code was not allowed.
//	* \deprecated
//	* This error return is deprecated as of CUDA 3.1. Device emulation mode was
//	* removed with the CUDA 3.1 release.
//	*/
//	cudaErrorMixedDeviceExecution         =     28,
//
//	/**
//	* This indicated an issue with calling API functions during the unload
//	* process of the CUDA runtime in prior releases.
//	* \deprecated
//	* This error return is deprecated as of CUDA 3.2.
//	*/
//	cudaErrorCudartUnloading              =     29,
//
//	/**
//	* This indicates that an unknown internal error has occurred.
//	*/
//	cudaErrorUnknown                      =     30,
//
//	/**
//	* This indicates that the API call is not yet implemented. Production
//	* releases of CUDA will never return this error.
//	*/
//	cudaErrorNotYetImplemented            =     31,
//
//	/**
//	* This indicated that an emulated device pointer exceeded the 32-bit address
//	* range.
//	* \deprecated
//	* This error return is deprecated as of CUDA 3.1. Device emulation mode was
//	* removed with the CUDA 3.1 release.
//	*/
//	cudaErrorMemoryValueTooLarge          =     32,
//
//	/**
//	* This indicates that a resource handle passed to the API call was not
//	* valid. Resource handles are opaque types like ::cudaStream_t and
//	* ::cudaEvent_t.
//	*/
//	cudaErrorInvalidResourceHandle        =     33,
//
//	/**
//	* This indicates that asynchronous operations issued previously have not
//	* completed yet. This result is not actually an error, but must be indicated
//	* differently than ::cudaSuccess (which indicates completion). Calls that
//	* may return this value include ::cudaEventQuery() and ::cudaStreamQuery().
//	*/
//	cudaErrorNotReady                     =     34,
//
//	/**
//	* This indicates that the installed NVIDIA CUDA driver is older than the
//	* CUDA runtime library. This is not a supported configuration. Users should
//	* install an updated NVIDIA display driver to allow the application to run.
//	*/
//	cudaErrorInsufficientDriver           =     35,
//
//	/**
//	* This indicates that the user has called ::cudaSetDevice(),
//	* ::cudaSetValidDevices(), ::cudaSetDeviceFlags(),
//	* ::cudaD3D9SetDirect3DDevice(), ::cudaD3D10SetDirect3DDevice,
//	* ::cudaD3D11SetDirect3DDevice(), * or ::cudaVDPAUSetVDPAUDevice() after
//	* initializing the CUDA runtime by calling non-device management operations
//	* (allocating memory and launching kernels are examples of non-device
//	* management operations). This error can also be returned if using
//	* runtime/driver interoperability and there is an existing ::CUcontext
//	* active on the host thread.
//	*/
//	cudaErrorSetOnActiveProcess           =     36,
//
//	/**
//	* This indicates that the surface passed to the API call is not a valid
//	* surface.
//	*/
//	cudaErrorInvalidSurface               =     37,
//
//	/**
//	* This indicates that no CUDA-capable devices were detected by the installed
//	* CUDA driver.
//	*/
//	cudaErrorNoDevice                     =     38,
//
//	/**
//	* This indicates that an uncorrectable ECC error was detected during
//	* execution.
//	*/
//	cudaErrorECCUncorrectable             =     39,
//
//	/**
//	* This indicates that a link to a shared object failed to resolve.
//	*/
//	cudaErrorSharedObjectSymbolNotFound   =     40,
//
//	/**
//	* This indicates that initialization of a shared object failed.
//	*/
//	cudaErrorSharedObjectInitFailed       =     41,
//
//	/**
//	* This indicates that the ::cudaLimit passed to the API call is not
//	* supported by the active device.
//	*/
//	cudaErrorUnsupportedLimit             =     42,
//
//	/**
//	* This indicates that multiple global or constant variables (across separate
//	* CUDA source files in the application) share the same string name.
//	*/
//	cudaErrorDuplicateVariableName        =     43,
//
//	/**
//	* This indicates that multiple textures (across separate CUDA source
//	* files in the application) share the same string name.
//	*/
//	cudaErrorDuplicateTextureName         =     44,
//
//	/**
//	* This indicates that multiple surfaces (across separate CUDA source
//	* files in the application) share the same string name.
//	*/
//	cudaErrorDuplicateSurfaceName         =     45,
//
//	/**
//	* This indicates that all CUDA devices are busy or unavailable at the current
//	* time. Devices are often busy/unavailable due to use of
//	* ::cudaComputeModeExclusive, ::cudaComputeModeProhibited or when long
//	* running CUDA kernels have filled up the GPU and are blocking new work
//	* from starting. They can also be unavailable due to memory constraints
//	* on a device that already has active CUDA work being performed.
//	*/
//	cudaErrorDevicesUnavailable           =     46,
//
//	/**
//	* This indicates that the device kernel image is invalid.
//	*/
//	cudaErrorInvalidKernelImage           =     47,
//
//	/**
//	* This indicates that there is no kernel image available that is suitable
//	* for the device. This can occur when a user specifies code generation
//	* options for a particular CUDA source file that do not include the
//	* corresponding device configuration.
//	*/
//	cudaErrorNoKernelImageForDevice       =     48,
//
//	/**
//	* This indicates that the current context is not compatible with this
//	* the CUDA Runtime. This can only occur if you are using CUDA
//	* Runtime/Driver interoperability and have created an existing Driver
//	* context using the driver API. The Driver context may be incompatible
//	* either because the Driver context was created using an older version 
//	* of the API, because the Runtime API call expects a primary driver 
//	* contextand the Driver context is not primary, or because the Driver 
//	* context has been destroyed. Please see \ref CUDART_DRIVER "Interactions 
//	* with the CUDA Driver API" for more information.
//	*/
//	cudaErrorIncompatibleDriverContext    =     49,
//
//	/**
//	* This error indicates that a call to ::cudaDeviceEnablePeerAccess() is
//	* trying to re-enable peer addressing on from a context which has already
//	* had peer addressing enabled.
//	*/
//	cudaErrorPeerAccessAlreadyEnabled     =     50,
//
//	/**
//	* This error indicates that ::cudaDeviceDisablePeerAccess() is trying to 
//	* disable peer addressing which has not been enabled yet via 
//	* ::cudaDeviceEnablePeerAccess().
//	*/
//	cudaErrorPeerAccessNotEnabled         =     51,
//
//	/**
//	* This indicates that a call tried to access an exclusive-thread device that 
//	* is already in use by a different thread.
//	*/
//	cudaErrorDeviceAlreadyInUse           =     54,
//
//	/**
//	* This indicates profiler has been disabled for this run and thus runtime 
//	* APIs cannot be used to profile subsets of the program. This can 
//	* happen when the application is running with external profiling tools
//	* like visual profiler.
//	*/
//	cudaErrorProfilerDisabled             =     55,
//
//	/**
//	* This indicates profiler has not been initialized yet. cudaProfilerInitialize()
//	* must be called before calling cudaProfilerStart and cudaProfilerStop to 
//	* initialize profiler.
//	*/
//	cudaErrorProfilerNotInitialized       =     56,
//
//	/**
//	* This indicates profiler is already started. This error can be returned if 
//	* cudaProfilerStart() is called multiple times without subsequent call
//	* to cudaProfilerStop().
//	*/
//	cudaErrorProfilerAlreadyStarted       =     57,
//
//	/**
//	* This indicates profiler is already stopped. This error can be returned if 
//	* cudaProfilerStop() is called without starting profiler using cudaProfilerStart().
//	*/
//	cudaErrorProfilerAlreadyStopped       =    58,
//
//	/**
//	* This indicates an internal startup failure in the CUDA runtime.
//	*/
//	cudaErrorStartupFailure               =   0x7f,
//
//	/**
//	* Any unhandled CUDA driver error is added to this value and returned via
//	* the runtime. Production releases of CUDA should not return such errors.
//	*/
//	cudaErrorApiFailureBase               =  10000
//};

using namespace GIEngine;

KDTreeStructureCUDA* Raytracer::ConvertIntoKDTreeCUDA( GIScene *rtScene, KDTreeStructure *KDTree )
{
	int NodeCount = KDTree->GetNodeSize();
	int TriangleCount = KDTree->GetTriangleCount();
	int TriangleIndexCount = KDTree->GetTriangleIndexCount();
	
	KDTreeStructureCUDA *KDTreeCUDA = new KDTreeStructureCUDA();
	if( !KDTreeCUDA->Initialize( NodeCount, TriangleCount, TriangleIndexCount ) )
	{
		delete KDTreeCUDA;
		return NULL;
	}

	cuKDTreeTriangle *TriangleArray = KDTreeCUDA->GetTriangleArray();
	for( int i = 0; i < TriangleCount; i++ )
	{
		TriangleArray[i] = rtScene->GetTriAccelList()[i];
	}

	unsigned int *TriangleIndexArray = KDTreeCUDA->GetTriangleIndexArray();

	struct traversalNode
	{
		traversalNode( GIKDTreeNode *_node, int _index ) { node = _node; index = _index; }
		GIKDTreeNode* node;
		int index;
	};
	
	std::stack<traversalNode> TraversalQueue;

	int nodeCount = 0;
	int nodeIndex = 0;
	int triangleIndex = 0;
	
	// TODO: 현재 GPU가 가질 수 있는 최대 노드 크기를 미리 비교??
	
	// breadth first search
	TraversalQueue.push( traversalNode( KDTree->GetRootNode(), nodeIndex++ ) );

	while( !TraversalQueue.empty() )
	{
		const traversalNode curTravNode = TraversalQueue.top();
		TraversalQueue.pop();

		assert( curTravNode.node != NULL );

		cuKDTreeNode *curCudaNode = KDTreeCUDA->GetNode(curTravNode.node->NodeNum);
		
		if( curTravNode.node->IsLeafNode() )
		{
			// leaf node
			curCudaNode->SetLeafNode( triangleIndex, curTravNode.node->TriangleCount );
			for( int i = 0; i < curTravNode.node->TriangleCount; i++ )
			{
				TriangleIndexArray[triangleIndex+i] = curTravNode.node->TriangleIndexArray[i];
			}
			triangleIndex += curTravNode.node->TriangleCount;
		}
		else
		{
			// internal node
			int leftNodeIndex = NULL_NODE_INDEX;
			int rightNodeIndex = NULL_NODE_INDEX;

			if( curTravNode.node->LeftNode != NULL )
				leftNodeIndex = nodeIndex++;
			if( curTravNode.node->RightNode != NULL )
				rightNodeIndex = nodeIndex++;

			nodeCount += 2;

			if( curTravNode.node->LeftNode != NULL )
				leftNodeIndex = curTravNode.node->LeftNode->NodeNum;
			if( curTravNode.node->RightNode != NULL )
				rightNodeIndex = curTravNode.node->RightNode->NodeNum;
			curCudaNode->SetInternalNode( leftNodeIndex, rightNodeIndex );
			//curCudaNode->SetInternalNode( leftNodeIndex, rightNodeIndex );

			if( curTravNode.node->LeftNode != NULL )
				TraversalQueue.push( traversalNode( curTravNode.node->LeftNode, leftNodeIndex ) );
			if( curTravNode.node->RightNode != NULL )
				TraversalQueue.push( traversalNode( curTravNode.node->RightNode, rightNodeIndex ) );
		}
	}

	assert( nodeIndex == NodeCount );

	//KDTreeCPU->
	
	return KDTreeCUDA;
}

bool Raytracer::InitializeKDTreeForCUDA( KDTreeStructureCUDA* KDTreeCUDA )
{
	cuKDTree *DeviceKDTree = CreateDeviceKDTreeAllocateKDTreeMemory( KDTreeCUDA->GetNodeCount(), KDTreeCUDA->GetNodeArray(), 
		KDTreeCUDA->GetTriangleCount(), KDTreeCUDA->GetTriangleArray(), 
		KDTreeCUDA->GetTriangleIndexCount(), KDTreeCUDA->GetTriangleIndexArray() );

	if( DeviceKDTree == NULL )
		return false;
	KDTreeCUDA->SetDeviceKDTree( DeviceKDTree );	
	return true;
}

//void Raytracer::ShootRaysCUDA( GIHit *outHits, GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, const GIRay *RayArray )
//{
//	cuKDTree *DeviceKDTree = KDTreeCUDA->GetDeviceKDTree();
//	// TODO: 삼각형 포함, GIScene 도 바꿔야함! KDTree 그대로 당연히 못씀.
//	assert( DeviceKDTree != NULL );
//	if( DeviceKDTree == NULL )
//		return;
//	cuShootRays( DeviceKDTree, RayCount, RayArray, outHits );
//}

//void Raytracer::SampleColorCUDA( GIScene *pScene, KDTreeStructure *KDTree, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray )
//{
//	cuKDTree *DeviceKDTree = KDTreeCUDA->GetDeviceKDTree();
//	// TODO: 삼각형 포함, GIScene 도 바꿔야함! KDTree 그대로 당연히 못씀.
//	assert( DeviceKDTree != NULL );
//	if( DeviceKDTree == NULL )
//		return;
//	cuShootRays( DeviceKDTree, RayCount, RayArray, outHits );
//}
//
//void Raytracer::SampleDistanceCUDA( GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, GIRay *RayArray, float *outDistanceArray )
//{
//	cuKDTree *DeviceKDTree = KDTreeCUDA->GetDeviceKDTree();
//	assert( DeviceKDTree != NULL );
//	if( DeviceKDTree == NULL )
//		return;
//	cuShootRays( DeviceKDTree, RayCount, RayArray, outHits );
//}
//
void Raytracer::SampleHitCUDA( GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, GIRay *RayArray, GIHit *outHitArray )
{
	cuKDTree *DeviceKDTree = KDTreeCUDA->GetDeviceKDTree();
	assert( DeviceKDTree != NULL );
	if( DeviceKDTree == NULL )
		return;
	cuShootRays( DeviceKDTree, RayCount, RayArray, outHitArray );
}