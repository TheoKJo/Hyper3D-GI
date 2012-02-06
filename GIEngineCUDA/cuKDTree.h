/*!
 * \file		cuKDTree.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/01/29
 * created:		2012/01/29 16:52
 * copyright	Hybrid (creavol@gmail.com)
 */

#ifndef CUDA_KD_TREE_H
#define CUDA_KD_TREE_H

#include "cuKDTreeNode.h"
// only for struct RtTriAcce?
#include "../GIEngineCore/Global.h"

typedef RtTriAccel cuKDTreeTriangle;

// TODO: 이게 사실상 Scene에 가까워져버렸는데;;
struct cuKDTree
{
	unsigned int NodeCount;
	cuKDTreeNode *NodeArray;
	unsigned int TriangleCount;
	unsigned int *TriangleIndexArray;
	unsigned int TriangleIndexCount;
	cuKDTreeTriangle *TriangleArray;
};


#endif