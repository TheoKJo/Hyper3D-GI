/*!
 * \file		KDTreeGPU.h
 * \brief		Structures for CUDA
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/01/22
 * created:		2012/01/22 15:51
 * copyright	Hybrid (creavol@gmail.com)
 */

#ifndef CUDA_TREE_NODE_H
#define CUDA_TREE_NODE_H

#define NULL_NODE_INDEX -1


struct cuKDTreeNode
{
	enum NODE_TYPE {
		LEAF_NODE, 
		INTERNAL_NODE, 
	} NodeType;

	void SetInternalNode( int leftNodeIndex, int rightNodeIndex )
	{
		NodeType = INTERNAL_NODE;
		index0 = leftNodeIndex;
		index1 = rightNodeIndex;
	}
	void SetLeafNode( int triangleStartIndex, int triangleCount )
	{
		NodeType = LEAF_NODE;
		index0 = triangleStartIndex;
		index1 = triangleCount;
	}

	void SetTriangleStartIndex( int startIndex ) { index0 = startIndex; }
	void SetTriangleCount( int triangleCount) { index1 = triangleCount; }
	int GetTriangleStartIndex() { return index0; }
	int GetTriangleCount() { return index1; }

	int GetLeftNodeIndex() { return index0; };
	int GetRightNodeIndex() { return index1; };

private:
	int index0;
	int index1;
};

#endif // CUDA_TREE_NODE_H
