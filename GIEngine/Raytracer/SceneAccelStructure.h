/*!
 * \file		SceneAccelStructure.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/01/29
 * created:		2012/01/29 16:14
 * copyright	Hybrid (creavol@gmail.com)
 */

class KDTreeStructure;
class KDTreeStructureCUDA;


//namespace GIEngine { namespace Raytracer {

class SceneAccelStructure
{
public:
	SceneAccelStructure()
		: m_KDTree(NULL), m_KDTreeCUDA(NULL)
	{
		
	}

	void SetKDTree( KDTreeStructure *KDtree ) { m_KDTree = KDtree; }
	void SetKDTreeCUDA( KDTreeStructureCUDA *KDtree ) { m_KDTreeCUDA = KDtree; }

	KDTreeStructure* GetKDTree() { return m_KDTree; }
	KDTreeStructureCUDA* GetKDTreeCUDA() { return m_KDTreeCUDA; }

private:
	// TODO: BHV, ...
	KDTreeStructure *m_KDTree;
	KDTreeStructureCUDA *m_KDTreeCUDA;
};

//};};