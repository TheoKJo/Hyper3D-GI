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
#include "GIScene.h"

typedef void (*ShadingFunction)( RtScene *rtScene, const GIRay &Ray, const GIHit &Hit, int MaxDepth, GIVector4 *outColor );

template<class T>
class IrradianceVolume;
class AmbientCube;
template<unsigned int order>
class SphericalHarmonics;
class AmbientCubeIrradianceVolume;

class SampleArray;
class SphericalSampleArray;
class GICamera;
struct GIHit;

///*!
// * NiNode의 Scene 은 외부에서 Raytracer로 포인터만 가르키는 aggregation 형태를 따른다.
// * 반면 RtScene 타입의 mRTScene 는 외부에서 감춰지는 의도로 존재한다.
// * mRTScene 는 외부에서 받은 NiNode 타입의 Scene으로부터 정보를 만들어내며, 외부에서 개별적으로 생성하는 것은 금지한다.
// */
//void Initialize( NiNode *pScene );

/*!
 * GI 관련 계산을 담당하는 클래스
 * \date 2011.02.07
 * \author Hybrid	
 */
namespace GIEngine
{
	class GISceneManager
	{
	public:
		virtual ~GISceneManager() {}
		virtual GIScene* CreateScene() = 0;
	};

	bool Initialize();
	void Finalize();

// Ray Generation
	void GenerateRays( GICamera *pCamera, GIRay *outRayArray );
	void GenerateSphericalSampleArray( const GIVector3 &Position, SphericalSampleArray *inoutSphericalSampleArray );
	void GenerateSphericalSampleArray( const GIVector3 &Position, const GIVector3 &Normal, SphericalSampleArray *inoutSphericalSampleArray );
	void GenerateHemisphericalSampleArray( const GIVector3 &Position, const GIVector3 &Normal, SphericalSampleArray *inoutSphericalSampleArray );

// Scene & Rendering
	bool BuildAccStructure( GIScene *Scene, const char *strStructureFilename );
	//void Render( NiCamera *pCamera, unsigned int Width, unsigned int Height, /* AcclStructure, */ float *outPixelData, unsigned int ThreadCount = 1 );
	void Render( unsigned int ThreadCount, GIScene *pScene, GICamera *pCamera, GIVector3 *outPixelData );

// Sampling
	// TODO: CPU, GPU 같이 사용할 함수로 바꿔야함
	void SampleColor( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray );
	void SampleDistance( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, float *outDistanceArray );
	void SampleHit( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, GIHit *outHitArray );
	void UserDefinedShading( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray, ShadingFunction pfnShadingFunction );

// Irradiance
	GIVector3 CalcIrradiance( GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, unsigned int RayCount, unsigned int ThreadCount );

// Ambient Cube
	//! AmbientCubeVolume 내부 데이터를 계산하는 함수.
	//! 단순히 쓰레드를 관리하고 루프 도는 역할만 함.
	void CalcAmbientCubeVolume( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *pScene, AmbientCubeIrradianceVolume *AmbientCubeVolume );

	/*!
	 * \param SamplingCount Irradiance 하나당 샘플링 개수(6방향이므로, 6*SamplingCount)
	 */
	void CalcAmbientCube( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, AmbientCube *ouAmbientCube );

	void CalcAmbientOcclusion( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, float *outAmbientOcclusion );

	namespace SHEngine {};

// Setting & Options
	void SetRayShootEpsilon( float epsilon );
	float GetRayShootEpsilon();
};