/*!
 * \file		GIEngine.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:13
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

//#include "Raytracer/Raytracer.h"
#include "Scene.h"

///*!
// * NiNode의 Scene 은 외부에서 Raytracer로 포인터만 가르키는 aggregation 형태를 따른다.
// * 반면 GIScene 타입의 mRTScene 는 외부에서 감춰지는 의도로 존재한다.
// * mRTScene 는 외부에서 받은 NiNode 타입의 Scene으로부터 정보를 만들어내며, 외부에서 개별적으로 생성하는 것은 금지한다.
// */
//void Initialize( NiNode *pScene );

struct GIHit;

/*!
 * GI 관련 계산을 담당하는 클래스
 * \date 2011.02.07
 * \author Hybrid	
 */
namespace GIEngine
{
	struct RaytracerOption;

	class SceneAccelStructure;
	class KDTreeStructure;

	template<class T>
	class IrradianceVolume;
	class AmbientCube;
	template<unsigned int order>
	class SphericalHarmonics;
	class AmbientCubeIrradianceVolume;

	class SampleArray;
	class SphericalSampleArray;
	class GICamera;

	typedef void (*ShadingFunction)( GIScene *pScene, KDTreeStructure *KDTree, const GIRay &Ray, const GIHit &Hit, int MaxDepth, GIVector4 *outColor );
	//typedef void (*ShadingFunction)( GIScene *rtScene, SceneAccelStructure *AccelStructure, const GIRay &Ray, const GIHit &Hit, int MaxDepth, GIVector4 *outColor );


	class GISceneManager
	{
	public:
		virtual ~GISceneManager() {}
		virtual GIScene* CreateScene() = 0;
	};

	struct RaytracerOption
	{
		RaytracerOption()
			: PassThroughBackHit(true), PassThroughSelfHit(true), RayEpsilon(0.00001f), 
			BuildDeviceTypeFlag(RDT_CPU), RenderingDeviceType(RDT_CPU), NumberOfThreads(4)
		{
		}

		bool PassThroughBackHit;
		bool PassThroughSelfHit;
		float RayEpsilon;

		enum RAYTRACER_DEVICE_TYPE
		{
			RDT_UNKNOWN, 
			RDT_CPU = 1, 
			RDT_CUDA = 1<<1, 
			RDT_OPENCL = 1<<2
		};

		// TODO: Display Ouput도 고려.
		int BuildDeviceTypeFlag;
		RAYTRACER_DEVICE_TYPE RenderingDeviceType;

		// for CPU
		unsigned int NumberOfThreads;
	};

	void SetGlobalOption( const RaytracerOption &Option );
	RaytracerOption GetGlobalOption();

	bool Initialize();
	void Finalize();

// Ray Generation
	void GenerateRays( GICamera *pCamera, GIRay *outRayArray );
	void GenerateSphericalSampleArray( const GIVector3 &Position, SphericalSampleArray *inoutSphericalSampleArray );
	void GenerateSphericalSampleArray( const GIVector3 &Position, const GIVector3 &Normal, SphericalSampleArray *inoutSphericalSampleArray );
	void GenerateHemisphericalSampleArray( const GIVector3 &Position, const GIVector3 &Normal, SphericalSampleArray *inoutSphericalSampleArray );

// Scene & Rendering
	SceneAccelStructure* BuildAccStructure( GIScene *Scene, const char *strStructureFilename );
	//void Render( NiCamera *pCamera, unsigned int Width, unsigned int Height, /* AcclStructure, */ float *outPixelData, unsigned int ThreadCount = 1 );
	void Render( GIScene *pScene, SceneAccelStructure *AccelStructure, GICamera *pCamera, GIVector3 *outPixelData );
	void RenderCPU( unsigned int ThreadCount, GIScene *pScene, SceneAccelStructure *AccelStructure, GICamera *pCamera, GIVector3 *outPixelData );

// Sampling
	void SampleColor( GIScene *pScene, SceneAccelStructure *AccelStructure, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray );
	void SampleDistance( GIScene *pScene, SceneAccelStructure *AccelStructure, unsigned int RayCount, GIRay *RayArray, float *outDistanceArray );
	void SampleHit( GIScene *pScene, SceneAccelStructure *AccelStructure, unsigned int RayCount, GIRay *RayArray, GIHit *outHitArray );
	void UserDefinedShading( GIScene *pScene, SceneAccelStructure *AccelStructure, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray, ShadingFunction pfnShadingFunction );

	namespace Raytracer {
		void SampleColorCPU( unsigned int ThreadCount, GIScene *pScene, KDTreeStructure *KDTree, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray );
		void SampleDistanceCPU( unsigned int ThreadCount, GIScene *pScene, KDTreeStructure *KDTree, unsigned int RayCount, GIRay *RayArray, float *outDistanceArray );
		void SampleHitCPU( unsigned int ThreadCount, GIScene *pScene, KDTreeStructure *KDTree, unsigned int RayCount, GIRay *RayArray, GIHit *outHitArray );
		void UserDefinedShadingCPU( unsigned int ThreadCount, GIScene *pScene, KDTreeStructure *KDTree, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray, ShadingFunction pfnShadingFunction );
	};

// Irradiance
	GIVector3 CalcIrradiance( GIScene *Scene, SceneAccelStructure *AccelStructure, const GIVector3 &Position, const GIVector3 &Normal, unsigned int RayCount );

// Ambient Cube
	//! AmbientCubeVolume 내부 데이터를 계산하는 함수.
	//! 단순히 쓰레드를 관리하고 루프 도는 역할만 함.
	void CalcAmbientCubeVolume( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *pScene, SceneAccelStructure *AccelStructure, AmbientCubeIrradianceVolume *AmbientCubeVolume );

	/*!
	 * \param SamplingCount Irradiance 하나당 샘플링 개수(6방향이므로, 6*SamplingCount)
	 */
	void CalcAmbientCube( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, SceneAccelStructure *AccelStructure, const GIVector3 &Position, AmbientCube *ouAmbientCube );

	void CalcAmbientOcclusion( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, SceneAccelStructure *AccelStructure, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, float *outAmbientOcclusion );

	namespace SHEngine {};

// Setting & Options
	void SetRayShootEpsilon( float epsilon );
	float GetRayShootEpsilon();
};