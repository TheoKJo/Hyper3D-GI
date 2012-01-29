/*!
 * \file		KDTreeGPU.h
 * \brief		Structures for CUDA
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/01/22
 * created:		2012/01/22 15:51
 * copyright	Hybrid (creavol@gmail.com)
 */

#ifndef KD_TREE_GPU_H
#define KD_TREE_GPU_H

struct KDTreeNodeGPU
{
	enum NODE_TYPE {
		LEAF_NODE, 
		EMPTY_NODE, 
		INTERNAL_NODE 
	} NodeType;

	void SetTriangleStartIndex( int startIndex ) { index0 = startIndex; }
	void SetTriangleCount( int triangleCount) { index1 = triangleCount; }
	int GetTriangleStartIndex() { return index0; }
	int GetTriangleCount() { return index1; }

	int GetLeftNodeIndex() { return index0 };
	int GetRightNodeIndex() { return index1 };

	int index0;
	int index1;
};

/*
 * struct KDTreeGPU
 */
struct KDTreeGPU
{
	sdfjasdklfjasdf
		CUDA 함수 버전과
		CPU 버전을 같이 만들어야할듯.
		아니면 만들어서 그 안에서둘다 가지고 있던지.
	KDTreeGPU() : m_NodeArray(NULL) {}
	void CreateNodes( unsigned int NodeCount )
	{
		if( m_NodeArray != NULL )
			delete[] m_NodeArray;
	}

	KDTreeNodeGPU *m_NodeArray;
};

#endif // KD_TREE_GPU_H

