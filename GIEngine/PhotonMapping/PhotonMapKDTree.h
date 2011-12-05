/*!
 * \file		PhotonMapKDTree.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:14
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "Global.h"
//#include "Structures.h"

#include <vector>
#include <assert.h>

namespace GIEngine { 
	class GIScene;
	namespace PhotonMapping {

class GIPhoton;

class GIPhotonMapKDTreeNode
{
public:
	GIPhotonMapKDTreeNode() 
		: PhotonIndexArray(NULL), PhotonCount(0), NodeNum(-1), LeftNode(NULL), RightNode(NULL)
	{
	}
	unsigned int *PhotonIndexArray;

	GIPhotonMapKDTreeNode *LeftNode;
	GIPhotonMapKDTreeNode *RightNode;

	unsigned int PhotonCount;
	int NodeNum;

	bool IsLeafNode() { return bLeafNode; }
	bool IsInternalNode() { return !bLeafNode; }

	//! for debugging (assertion)
	bool IsValid()
	{
		if( NodeNum < 0 )
			return false;
		if( SplitAxis < -1 || 2 < SplitAxis )
			return false;
		if( IsLeafNode() )
		{
			//if( SplitAxis != -1 )
			//	return false;
			if( LeftNode != NULL || RightNode != NULL )
				return false;
		}
		else
		{
			if( SplitAxis == -1 )
				return false;
			if( LeftNode == NULL && RightNode == NULL )
				return false;
		}
		return true;
	}

	bool bLeafNode;
	int SplitAxis;
	float MinPosition;
	float SplitPosition;
	float MaxPosition;
};

class RtSAHCost
{
public:
	RtSAHCost() : IsSplit(false), Axis(-1), SplitEdgeIndex(-1), 
		LeftCount(0), CrossPhotonCount(0), RightCount(0), 
		SplitPosition(0.0f), //!< TODO: \infty
		SplitCost(1e16f)		//!< TODO: \infty
	{}

	// 필요 없음.
	void ResetInvalid()
	{
		IsSplit = false; Axis = -1; SplitEdgeIndex = -1; LeftCount = 0; CrossPhotonCount = 0; RightCount = 0;
		SplitPosition = 0.0f; SplitCost = 1e16f;
	}

	bool IsSplit;
	int Axis;
	int SplitEdgeIndex;
	float SplitPosition;
	float SplitCost;

	int LeftCount;
	int CrossPhotonCount;
	int RightCount;

	int LeftOnlyCount;
	int RightOnlyCount;

	int PlanarCount;
};

class GIPhotonMapKDTree
{
public:
	GIPhotonMapKDTree();
	virtual ~GIPhotonMapKDTree();

	bool IsBuilt();
	void SetBuildParameters( int MinimumPhotonCount, float MinimumNodeLength );

	void ConstructKDTree( const std::vector<GIPhoton> &PhotonArray, unsigned int PhotonCount );
	void DestructTree();

	bool SaveToFile( const char *Filename );
	bool LoadFromFile( const char *Filename );

	GIPhotonMapKDTreeNode* GetRootNode() { return mRootNode; }
	int GetNodeSize() { return mNodeSize; }

	GIBoundingBox GetBoundingBox() { return mPhotonMapBoundingBox; }
	
private:
	//! ConstructKDTree 용 내부 함수
	GIPhotonMapKDTreeNode* CreateNode( const std::vector<unsigned int> &PhotonIndexList, const int PhotonCount, const GIBoundingBox &BoundingBox, int SplitAxis );
	void EvaluateCost( RtSAHCost &SAHCost, const std::vector<unsigned int> &PhotonIndexList, const int PhotonCount, const GIBoundingBox &BoundingBox, int Axis );

public:
	// Build 용
	int mMinimumPhotonCount;
	float mMinimumNodeLength;

private:
	int mCurNodePhotonIndex;	//!< 현재의 triangle index 들이 저장될 위치
	int mCurNodeSize; //!< for mKDTreeNodeSize

private:
	GIPhotonMapKDTreeNode *mRootNode;
	int mNodeSize;
	GIBoundingBox mPhotonMapBoundingBox;

// Internal Use Only
private:
	unsigned int mPhotonCount;
	GIPhotonMapKDTreeNode *mNodeArray;
	const std::vector<GIPhoton> *mPhotonArray;
};

};};