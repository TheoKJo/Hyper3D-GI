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
namespace GIEngine
{ 
	class GIScene;
	
	namespace PhotonMapping
	{
//		class GIPhotonMap;

		void GeneratePhotons( const GIVector3 &Position, const GIColor3 &Color, unsigned int PhotonCount, GIPhoton *outPhotonArray, SphericalCoordinateSamplingFunction pSphericalCoordinateSamplingFunction );

		GIPhotonMap* CreatePhotonMap( GIScene *inScene, unsigned int MaxPhotonPass );

		//! \return living photon이 한개라도 존재한다면 true, 모두 죽은 포톤일 경우 false.
		bool ShootPhotons( unsigned int Pass, GIScene *inScene, 
			GIPhoton *inPhotonArray, unsigned int inPhotonCount, 
			GIPhoton *outPhotonArray = NULL, unsigned int *outLivingPhotonCount = NULL );

		//! 직접 조명은 계산하지 않음.
		GIColor3 FinalGathering( GIScene *inScene, GIPhotonMap *inPhotonMap, const GIVector3 &Position, const GIVector3 &RayDirection/*, const GIHit &hitResult*/ );
		void Render( unsigned int ThreadCount, GIScene *pScene, GIPhotonMap *pPhotonMap, GICamera *pCamera, GIVector3 *outPixelData );
	};
};