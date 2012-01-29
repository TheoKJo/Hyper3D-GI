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

	// TODO: ���� üũ�� �ϰ�, ����� �ؽ�Ʈ�� ����(����?)�� �˷���.
	void CheckPerformance();

	KDTreeGPU* CreateKDTreeGPU( RtScene *rtScene );

	// TODO: Get, Set GPU Options (���� ���� �Ұ�? MultiGPU������ �����Ѱ�?)
	void ShootRays( GIHit *outHits, RtScene *rtScene, const GIRay *RayArray, unsigned int RayCount );

};};