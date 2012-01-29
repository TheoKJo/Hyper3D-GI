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
// * NiNode�� Scene �� �ܺο��� Raytracer�� �����͸� ����Ű�� aggregation ���¸� ������.
// * �ݸ� RtScene Ÿ���� mRTScene �� �ܺο��� �������� �ǵ��� �����Ѵ�.
// * mRTScene �� �ܺο��� ���� NiNode Ÿ���� Scene���κ��� ������ ������, �ܺο��� ���������� �����ϴ� ���� �����Ѵ�.
// */
//void Initialize( NiNode *pScene );

/*!
 * GI ���� ����� ����ϴ� Ŭ����
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
	// TODO: CPU, GPU ���� ����� �Լ��� �ٲ����
	void SampleColor( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray );
	void SampleDistance( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, float *outDistanceArray );
	void SampleHit( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, GIHit *outHitArray );
	void UserDefinedShading( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray, ShadingFunction pfnShadingFunction );

// Irradiance
	GIVector3 CalcIrradiance( GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, unsigned int RayCount, unsigned int ThreadCount );

// Ambient Cube
	//! AmbientCubeVolume ���� �����͸� ����ϴ� �Լ�.
	//! �ܼ��� �����带 �����ϰ� ���� ���� ���Ҹ� ��.
	void CalcAmbientCubeVolume( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *pScene, AmbientCubeIrradianceVolume *AmbientCubeVolume );

	/*!
	 * \param SamplingCount Irradiance �ϳ��� ���ø� ����(6�����̹Ƿ�, 6*SamplingCount)
	 */
	void CalcAmbientCube( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, AmbientCube *ouAmbientCube );

	void CalcAmbientOcclusion( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, float *outAmbientOcclusion );

	namespace SHEngine {};

// Setting & Options
	void SetRayShootEpsilon( float epsilon );
	float GetRayShootEpsilon();
};