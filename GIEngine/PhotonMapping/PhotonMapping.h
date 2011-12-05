/*!
 * \file		PhotonMapping.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:14
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "Photon.h"
#include "PhotonMap.h"

#include "../SampleFunction.h"

// 실제로 도달하지 말아야하는 PASS
// 여기까지 도달할 경우 이론(확률)과 맞지 않는 문제가 있다고 가정함.
#define MAX_PHOTON_SHOOTING_PASS 10

class GICamera;

/*! \namespace  PhotonMapping
 * \brief 
 * 
 * \author 김혁
 * \date  2011.08.22 ~
 */
namespace GIEngine {
	class GIScene;	
	namespace PhotonMapping {

struct PhotonMappingOption
{
	PhotonMappingOption()
		: GatheringRadius(0.0f), PhotonCountPerLight(0), MaxPhotonPass(2), PhotonIntensity(1.0f), 
		MaximumGatheringPhotonCount(0), ExcludeDirectPhoton(false)
	{
	}

	float GatheringRadius;
	unsigned int PhotonCountPerLight;
	unsigned int MaxPhotonPass;
	//! 상수로 둠.
	float PhotonIntensity;
	unsigned int MaximumGatheringPhotonCount;
	bool ExcludeDirectPhoton;

	bool IsValid()
	{
		if( GatheringRadius > 0.0f && 
			PhotonCountPerLight > 0 && 
			MaxPhotonPass > 01 )
			return true;
		return false;
	}
};

//! Uniform
//! Lambertian surface
inline void UniformSphericalCoordinateSamplingFunction( float xi1, float xi2, float *outTheta, float *outPhi )
{
	*outTheta = 2.0f * acos( sqrt( 1.0f - xi1 ) );
	*outPhi = 2.0f * FLOAT_PI * xi2;
}

void SetPhotonMappingOption( const PhotonMappingOption &Option );
PhotonMappingOption GetPhotonMappingOption();

void GeneratePhotons( const GIVector3 &Position, const GIColor3 &Color, unsigned int PhotonCount, GIPhoton *outPhotonArray, SphericalCoordinateSamplingFunction pSphericalCoordinateSamplingFunction );

// TODO: Option 을 매번 받아야함.
GIPhotonMap* CreatePhotonMap( GIScene *inScene );

//! \return living photon이 한개라도 존재한다면 true, 모두 죽은 포톤일 경우 false.
bool ShootPhotons( unsigned int Pass, GIScene *inScene, 
	GIPhoton *inPhotonArray, unsigned int inPhotonCount, 
	GIPhoton *outPhotonArray = NULL, unsigned int *outLivingPhotonCount = NULL );

//! 직접 조명은 계산하지 않음.
GIColor3 FinalGathering( GIScene *inScene, GIPhotonMap *inPhotonMap, const GIVector3 &Position, const GIVector3 &RayDirection/*, const GIHit &hitResult*/ );
void Render( unsigned int ThreadCount, GIScene *pScene, GIPhotonMap *pPhotonMap, GICamera *pCamera, GIVector3 *outPixelData );

};};