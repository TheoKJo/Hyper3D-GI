/*!
 * \file		KDTreeStructure.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "Global.h"
#include "Structures.h"

#include <vector>
#include <assert.h>

//struct RtEdge
//{
//	float position;
//	bool isLeft;
//	bool isPlanar;
//	unsigned int TriangleIndex;
//};

//struct RtEdge
//{
//	float position[3];
//	bool isLeft;
//	bool isPlanarArray[3];
//	unsigned int TriangleIndex;
//};

struct RtEdge
{
	/*RtEdge& operator=( const RtEdge &edge )
	{
		axis = edge.axis;
		position = edge.position;
		TriangleIndex = edge.TriangleIndex;
		bIsLeft = edge.bIsLeft;
		bIsPlanar = edge.bIsPlanar;
		return *this;
	}*/
	int axis;		// k
	float position;
	unsigned int TriangleIndex;

	bool bIsLeft;
	bool bIsPlanar;

	bool isPlanar() { return bIsPlanar; }
	bool isLeft() { return bIsLeft; }
	bool isRight() { return !bIsLeft; }

	void setLeft() { bIsLeft = true; }
	void setRight() { bIsLeft = false; }
	void setPlanar( bool flag = true ) { bIsPlanar = flag; }

	//enum TYPE {
	//	RIGHT_EDGE = 0, //!< End, Close
	//	PLANAR = 1, 
	//	LEFT_EDGE = 2 //!< Start, Open
	//};
	//TYPE type;

	//bool isPlanar() { return type == PLANAR; }
	//bool isLeft() { return type == LEFT_EDGE; }
	//bool isRight() { return type == RIGHT_EDGE; }
};

class TriangleBox
{
public:
	void Initialize( const GITriangle *Triangle )
	{
		m_BoxMin = GIVector3( 1e15f, 1e15f, 1e15f );
		m_BoxMax = GIVector3( -1e15f, -1e15f, -1e15f );

		for( int i = 0; i < 3; i++ )
		{
			if( m_BoxMin.array[i] > Triangle->vg0.Vertex.array[i] )
				m_BoxMin.array[i] = Triangle->vg0.Vertex.array[i];
			if( m_BoxMin.array[i] > Triangle->vg1.Vertex.array[i] )
				m_BoxMin.array[i] = Triangle->vg1.Vertex.array[i];
			if( m_BoxMin.array[i] > Triangle->vg2.Vertex.array[i] )
				m_BoxMin.array[i] = Triangle->vg2.Vertex.array[i];

			if( m_BoxMax.array[i] < Triangle->vg0.Vertex.array[i] )
				m_BoxMax.array[i] = Triangle->vg0.Vertex.array[i];
			if( m_BoxMax.array[i] < Triangle->vg1.Vertex.array[i] )
				m_BoxMax.array[i] = Triangle->vg1.Vertex.array[i];
			if( m_BoxMax.array[i] < Triangle->vg2.Vertex.array[i] )
				m_BoxMax.array[i] = Triangle->vg2.Vertex.array[i];
			assert( m_BoxMin.array[i] <= m_BoxMax.array[i] );

			bPlanar[i] = m_BoxMin.array[i] == m_BoxMax.array[i];
		}
	}

	GIVector3 m_BoxMin;
	GIVector3 m_BoxMax;
	bool bPlanar[3];

	bool isPlanar( int Axis ) { return bPlanar[Axis]; }
};

class RtKDTreeNode
{
public:
	RtKDTreeNode() 
		: TriangleIndexArray(NULL), TriangleCount(0), NodeNum(-1), LeftNode(NULL), RightNode(NULL)
	{
	}
	unsigned int *TriangleIndexArray;

	RtKDTreeNode *LeftNode;
	RtKDTreeNode *RightNode;

	int TriangleCount;
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
		LeftCount(0), CrossTriangleCount(0), RightCount(0), 
		SplitPosition(0.0f), //!< TODO: \infty
		SplitCost(1e16f)		//!< TODO: \infty
	{}

	// 필요 없음.
	void ResetInvalid()
	{
		IsSplit = false; Axis = -1; SplitEdgeIndex = -1; LeftCount = 0; CrossTriangleCount = 0; RightCount = 0;
		SplitPosition = 0.0f; SplitCost = 1e16f;
	}

	bool IsSplit;
	int Axis;
	int SplitEdgeIndex;
	float SplitPosition;
	float SplitCost;

	int LeftCount;
	int CrossTriangleCount;
	int RightCount;

	int LeftOnlyCount;
	int RightOnlyCount;

	int PlanarCount;
};

class KDTreeStructure : public RtAccelStructure
{
public:
	KDTreeStructure();
	virtual ~KDTreeStructure();

	void ConstructKDTree( const GITriangle *TriangleList, const int TriangleCount, const GIBoundingBox &BoundingBox );
	void DestructTree();

	bool SaveToFile( const char *Filename );
	bool LoadFromFile( const char *Filename );

	bool IsBuilt();
	void SetBuildParameters( const float &IntersectionCost, const float &TraversalCost, const int Minimum_TriangleCount );

	RtKDTreeNode* GetRootNode() { return m_RootNode; }
	int GetNodeSize() { return m_NodeSize; }

	GIBoundingBox GetBoundingBox() { return m_SceneBoundingBox; }
	
private:
	//! ConstructKDTree 용 내부 함수
	RtKDTreeNode* CreateNode( int StartEdgeIndex, const std::vector<unsigned int> &TriangleList, const int TriangleCount, const GIBoundingBox &BoundingBox );
	void EvaluateCost( RtSAHCost &SAHCost, const std::vector<unsigned int> &TriangleList, const int TriangleCount, const GIBoundingBox &BoundingBox, int Axis );
	int InitializeEdge( RtEdge *EdgeArray, const std::vector<unsigned int> &TriangleList, const int TriangleCount, int Axis );

public:
	// Build 용
	int m_MinimumTriangleCount;
	float m_IntersectionCost;
	float mTraversalCost;
private:
	int m_CurNodeTriangleIndex;	//!< 현재의 triangle index 들이 저장될 위치
	int m_CurNodeSize; //!< for mKDTreeNodeSize

private:
	RtKDTreeNode *m_RootNode;
	int m_NodeSize;
	GIBoundingBox m_SceneBoundingBox;
	
	//unsigned int 

// Internal Use Only
private:
	void SortEdgeArray( RtEdge *EdgeArray, int EdgeSize );
	void SortEdgeIndexArray();
	void TriangleToBoundingBox( GIVector3 &BoxMin, GIVector3 &BoxMax, const GITriangle *Triangle );	

	TriangleBox *m_TriangleBoxArray;

	int m_TriangleCount;
	RtKDTreeNode *m_NodeArray;
};
