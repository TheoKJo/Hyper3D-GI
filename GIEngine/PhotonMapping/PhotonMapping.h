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

// ������ �������� ���ƾ��ϴ� PASS
// ������� ������ ��� �̷�(Ȯ��)�� ���� �ʴ� ������ �ִٰ� ������.
#define MAX_PHOTON_SHOOTING_PASS 10

class GICamera;

/*! \namespace  PhotonMapping
 * \brief 
 * 
 * \author ����
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
	//! ����� ��.
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

// TODO: Option �� �Ź� �޾ƾ���.
GIPhotonMap* CreatePhotonMap( GIScene *inScene );

//! \return living photon�� �Ѱ��� �����Ѵٸ� true, ��� ���� ������ ��� false.
bool ShootPhotons( unsigned int Pass, GIScene *inScene, 
	GIPhoton *inPhotonArray, unsigned int inPhotonCount, 
	GIPhoton *outPhotonArray = NULL, unsigned int *outLivingPhotonCount = NULL );

//! ���� ������ ������� ����.
GIColor3 FinalGathering( GIScene *inScene, GIPhotonMap *inPhotonMap, const GIVector3 &Position, const GIVector3 &RayDirection/*, const GIHit &hitResult*/ );
void Render( unsigned int ThreadCount, GIScene *pScene, GIPhotonMap *pPhotonMap, GICamera *pCamera, GIVector3 *outPixelData );

};};