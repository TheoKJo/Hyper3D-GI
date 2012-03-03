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

// TODO: ���� üũ�� �ϰ�, ����� �ؽ�Ʈ�� ����(����?)�� �˷���.
void CheckPerformance();

namespace Raytracer {
	//cuKDTree* BuildKDTreeCUDA( GIScene *rtScene );

	// TODO: Convert ��� �̸��� �������� ����.
	KDTreeStructureCUDA* ConvertIntoKDTreeCUDA( GIScene *rtScene, KDTreeStructure *KDTree );

	bool InitializeKDTreeForCUDA( KDTreeStructureCUDA* KDTreeCUDA );

	// TODO: Get, Set GPU Options (���� ���� �Ұ�? MultiGPU������ �����Ѱ�?)
	//void ShootRaysCUDA( GIHit *outHits, GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, unsigned int RayCount, const GIRay *RayArray );

	//void SampleColorCUDA( GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, GIRay *RayArray, unsigned int RayCount, GIVector3 *outColorArray );
	//void SampleDistanceCUDA( GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, GIRay *RayArray, unsigned int RayCount, float *outDistanceArray );
	void SampleHitCUDA( GIScene *rtScene, KDTreeStructureCUDA *KDTreeCUDA, GIRay *RayArray, unsigned int RayCount, GIHit *outHitArray );

	// TODO: �Ƹ��� �ܺηκ��� ������ ������?
	//char* ErrorStringFromCURAError( cudaError );
};};