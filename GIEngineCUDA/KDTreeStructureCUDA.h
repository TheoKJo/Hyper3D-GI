/*!
 * \file		KDTreeStructureCUDA.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/01/29
 * created:		2012/01/29 16:21
 * copyright	Hybrid (creavol@gmail.com)
 */

struct cuKDTree;
struct cuKDTreeNode;

typedef GITriAccel cuKDTreeTriangle;

namespace GIEngine {

class KDTreeStructureCUDA
{
public:
	KDTreeStructureCUDA();
	~KDTreeStructureCUDA();

	/*!
	 * TriangleCountInNodes: sum of number of triangles in each (leaf) node
	 */
	bool Initialize( unsigned int NodeCount, unsigned int TriangleCount, unsigned int TriangleIndexCount );
	void Finalize();

	unsigned int GetNodeCount() { return m_NodeCount; }
	cuKDTreeNode *GetNodeArray() { return m_NodeArray; }
	cuKDTreeNode* GetNode( unsigned int NodeIndex );

	unsigned int GetTriangleCount() { return m_TriangleCount; }
	cuKDTreeTriangle* GetTriangleArray();
	unsigned int GetTriangleIndexCount() { return m_TriangleIndexCount; }
	unsigned int* GetTriangleIndexArray();

	//SetDeviceMemory

	void SetDeviceKDTree( cuKDTree *DeviceKDTree ) { m_DeviceKDTree = DeviceKDTree; }
	cuKDTree* GetDeviceKDTree() { return m_DeviceKDTree; }
	
private:
	unsigned int m_NodeCount;
	cuKDTreeNode *m_NodeArray;
	
	unsigned int m_TriangleIndexCount;
	unsigned int *m_TriangleIndexArray;

	unsigned int m_TriangleCount;
	cuKDTreeTriangle *m_TriangleArray;

	// for CUDA
	cuKDTree *m_DeviceKDTree;
};
};