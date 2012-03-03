/*!
 * \file		GIEngineCUDA.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/01/22
 * created:		2012/01/22 13:15
 * copyright	Hybrid (creavol@gmail.com)
 */

namespace GIEngine { 

class KDTreeStructure;
class KDTreeStructureCUDA;

bool InitializeCUDA();

// TODO: 성능 체크를 하고, 결과를 텍스트와 숫자(레벨?)로 알려줌.
void CheckPerformance();

namespace Raytracer {
	//cuKDTree* BuildKDTreeCUDA( GIScene *rtScene );

	// TODO: Convert 라는 이름이 적절하지 않음.
	KDTreeStructureCUDA* ConvertIntoKDTreeCUDA( GIScene *rtScene, KDTreeStructure *KDTree );

	bool InitializeKDTreeForCUDA( KDTreeStructureCUDA* KDTreeCUDA );

	// TODO: Get, Set GPU Options (동시 실행 불가? MultiGPU에서는 가능한가?)
	//void ShootRaysCUDA( GIHit *outHits, GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, const GIRay *RayArray );

	//void SampleColorCUDA( GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, GIRay *RayArray, unsigned int RayCount, GIVector3 *outColorArray );
	//void SampleDistanceCUDA( GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, GIRay *RayArray, unsigned int RayCount, float *outDistanceArray );
	void SampleHitCUDA( GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, GIRay *RayArray, unsigned int RayCount, GIHit *outHitArray );

	// TODO: 아마도 외부로부터 감추지 않을까?
	//char* ErrorStringFromCURAError( cudaError );
};};