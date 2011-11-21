#pragma once

#include "Global.h"
//#include "Structures.h"

#include <vector>
#include <assert.h>

namespace GIEngine { 
	class GIScene;
	namespace PhotonMapping {

class GIPhoton;

struct RtEdge
{
	/*RtEdge& operator=( const RtEdge &edge )
	{
		axis = edge.axis;
		position = edge.position;
		PhotonIndex = edge.PhotonIndex;
		bIsLeft = edge.bIsLeft;
		bIsPlanar = edge.bIsPlanar;
		return *this;
	}*/
	int axis;		// k
	float position;
	unsigned int PhotonIndex;

	bool bIsLeft;
	bool bIsPlanar;

	bool isPlanar() { return bIsPlanar; }
	bool isLeft() { return bIsLeft; }
	bool isRight() { return !bIsLeft; }

	void setLeft() { bIsLeft = true; }
	void setRight() { bIsLeft = false; }
	void setPlanar( bool flag = true ) { bIsPlanar = flag; }
};

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

	void ConstructKDTree( const GIPhoton *PhotonList, const int PhotonCount );
	void DestructTree();

	bool SaveToFile( const char *Filename );
	bool LoadFromFile( const char *Filename );

	bool IsBuilt();
	void SetBuildParameters( const float &IntersectionCost, const float &TraversalCost, const int MinimumPhotonCount );

	GIPhotonMapKDTreeNode* GetRootNode() { return mRootNode; }
	int GetNodeSize() { return mNodeSize; }

	GIBoundingBox GetBoundingBox() { return mSceneBoundingBox; }
	
private:
	//! ConstructKDTree 용 내부 함수
	GIPhotonMapKDTreeNode* CreateNode( int StartEdgeIndex, const std::vector<unsigned int> &PhotonList, const int PhotonCount, const GIBoundingBox &BoundingBox );
	void EvaluateCost( RtSAHCost &SAHCost, const std::vector<unsigned int> &PhotonList, const int PhotonCount, const GIBoundingBox &BoundingBox, int Axis );
	int InitializeEdge( RtEdge *EdgeArray, const std::vector<unsigned int> &PhotonList, const int PhotonCount, int Axis );

public:
	// Build 용
	int mMinimumPhotonCount;
	float mIntersectionCost;
	float mTraversalCost;
private:
	int mCurNodePhotonIndex;	//!< 현재의 triangle index 들이 저장될 위치
	int mCurNodeSize; //!< for mKDTreeNodeSize

private:
	GIPhotonMapKDTreeNode *mRootNode;
	int mNodeSize;
	GIBoundingBox mSceneBoundingBox;

// Internal Use Only
private:
	void SortEdgeArray( RtEdge *EdgeArray, int EdgeSize );
	void SortEdgeIndexArray();

	int mPhotonCount;
	GIPhotonMapKDTreeNode *mNodeArray;
};

};};