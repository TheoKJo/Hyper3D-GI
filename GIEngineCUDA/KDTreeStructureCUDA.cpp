#include "GIEngineCUDA_PCH.h"

#include "KDTreeStructureCUDA.h"

#include "cuKDTreeNode.h"

typedef RtTriAccel cuKDTreeTriangle;

KDTreeStructureCUDA::KDTreeStructureCUDA()
	: m_NodeCount(NULL), m_NodeArray(NULL), 
	m_TriangleCount(NULL), m_TriangleArray(NULL), m_TriangleIndexCount(NULL), m_TriangleIndexArray(NULL)
{
}

KDTreeStructureCUDA::~KDTreeStructureCUDA()
{
	Finalize();
}

/*!
 * TriangleCountInNodes: sum of number of triangles in each (leaf) node
 */
bool KDTreeStructureCUDA::Initialize( unsigned int NodeCount, unsigned int TriangleCount, unsigned int TriangleIndexCount )
{
	if( m_NodeArray != NULL )
		delete[] m_NodeArray;
	m_NodeArray = new cuKDTreeNode[NodeCount];
	m_NodeCount = NodeCount;

	if( m_TriangleArray != NULL )
		delete[] m_TriangleArray;
	m_TriangleArray = new cuKDTreeTriangle[TriangleCount];
	m_TriangleCount = TriangleCount;

	if( m_TriangleIndexArray != NULL )
		delete[] m_TriangleIndexArray;
	m_TriangleIndexArray = new unsigned int[TriangleIndexCount];
	m_TriangleIndexCount = TriangleIndexCount;		
	return true;
}

void KDTreeStructureCUDA::Finalize()
{
	if( m_NodeArray != NULL )
		delete[] m_NodeArray;
	if( m_TriangleArray != NULL )
		delete[] m_TriangleArray;
	if( m_TriangleIndexArray != NULL )
		delete[] m_TriangleIndexArray;
}

cuKDTreeNode* KDTreeStructureCUDA::GetNode( unsigned int NodeIndex )
{
	return &m_NodeArray[NodeIndex];
}

cuKDTreeTriangle* KDTreeStructureCUDA::GetTriangleArray()
{
	return m_TriangleArray;
}

unsigned int* KDTreeStructureCUDA::GetTriangleIndexArray()
{
	return m_TriangleIndexArray;
}
