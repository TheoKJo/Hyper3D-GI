/*!
 * \file		GIEngineCUDA.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/01/22
 * created:		2012/01/22 13:15
 * copyright	Hybrid (creavol@gmail.com)
 */

class KDTreeStructure;
class KDTreeStructureCUDA;

namespace GIEngine { 

bool InitializeCUDA();

// TODO: ���� üũ�� �ϰ�, ����� �ؽ�Ʈ�� ����(����?)�� �˷���.
void CheckPerformance();

namespace Raytracer {
	//cuKDTree* BuildKDTreeCUDA( RtScene *rtScene );

	// TODO: Convert ��� �̸��� �������� ����.
	KDTreeStructureCUDA* ConvertIntoKDTreeCUDA( RtScene *rtScene, KDTreeStructure *KDTree );

	bool InitializeKDTreeForCUDA( KDTreeStructureCUDA* KDTreeCUDA );

	// TODO: Get, Set GPU Options (���� ���� �Ұ�? MultiGPU������ �����Ѱ�?)
	void ShootRaysCUDA( GIHit *outHits, RtScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, const GIRay *RayArray );

	//void SampleColorCUDA( RtScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray );
	//void SampleDistanceCUDA( RtScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, GIRay *RayArray, float *outDistanceArray );
	void SampleHitCUDA( RtScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, GIRay *RayArray, GIHit *outHitArray );

	// TODO: �Ƹ��� �ܺηκ��� ������ ������?
	//char* ErrorStringFromCURAError( cudaError );
};};