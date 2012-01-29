/*!
 * \file		GIEngineCUDA.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/01/22
 * created:		2012/01/22 13:15
 * copyright	Hybrid (creavol@gmail.com)
 */

namespace GIEngine { namespace Raytracer {

	class KDTreeGPU;

	// TODO: 성능 체크를 하고, 결과를 텍스트와 숫자(레벨?)로 알려줌.
	void CheckPerformance();

	KDTreeGPU* CreateKDTreeGPU( RtScene *rtScene );

	// TODO: Get, Set GPU Options (동시 실행 불가? MultiGPU에서는 가능한가?)
	void ShootRays( GIHit *outHits, RtScene *rtScene, const GIRay *RayArray, unsigned int RayCount );

};};