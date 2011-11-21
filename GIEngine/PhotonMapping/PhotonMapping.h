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
namespace GIEngine
{ 
	class GIScene;
	
	namespace PhotonMapping
	{
//		class GIPhotonMap;

		void GeneratePhotons( const GIVector3 &Position, const GIColor3 &Color, unsigned int PhotonCount, GIPhoton *outPhotonArray, SphericalCoordinateSamplingFunction pSphericalCoordinateSamplingFunction );

		GIPhotonMap* CreatePhotonMap( GIScene *inScene, unsigned int MaxPhotonPass );

		//! \return living photon�� �Ѱ��� �����Ѵٸ� true, ��� ���� ������ ��� false.
		bool ShootPhotons( unsigned int Pass, GIScene *inScene, 
			GIPhoton *inPhotonArray, unsigned int inPhotonCount, 
			GIPhoton *outPhotonArray = NULL, unsigned int *outLivingPhotonCount = NULL );

		//! ���� ������ ������� ����.
		GIColor3 FinalGathering( GIScene *inScene, GIPhotonMap *inPhotonMap, const GIVector3 &Position, const GIVector3 &RayDirection/*, const GIHit &hitResult*/ );
		void Render( unsigned int ThreadCount, GIScene *pScene, GIPhotonMap *pPhotonMap, GICamera *pCamera, GIVector3 *outPixelData );
	};
};