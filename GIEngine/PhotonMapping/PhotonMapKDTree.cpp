/*!
 * \file		PhotonMapKDTree.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/12/05
 * created:		2011/12/05 15:55
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"
#include "../GIEnginePCH.h"

#include "PhotonMapKDTree.h"
#include "Photon.h"

using namespace GIEngine;
using namespace PhotonMapping;

GIPhotonMapKDTree::GIPhotonMapKDTree()
	: mMinimum_PhotonCount(5), mMinimumNodeLength(0), m_CurNodePhotonIndex(0), m_CurNodeSize(0), 
	m_RootNode(NULL), m_NodeSize(0), m_PhotonCount(0), m_NodeArray(NULL), m_PhotonArray(NULL)
{
	
	
}

GIPhotonMapKDTree::~GIPhotonMapKDTree()
{
	m_RootNode = NULL;
	SafeDeleteArray( &m_NodeArray );
	m_PhotonArray = NULL;
}

bool GIPhotonMapKDTree::IsBuilt()
{
	return m_PhotonCount > 0 && m_PhotonArray != NULL;
}

void GIPhotonMapKDTree::SetBuildParameters( int Minimum_PhotonCount, float MinimumNodeLength )
{
	mMinimum_PhotonCount = Minimum_PhotonCount;
	mMinimumNodeLength = MinimumNodeLength;
}

void GIPhotonMapKDTree::ConstructKDTree( const std::vector<GIPhoton> &PhotonArray, unsigned int PhotonCount )
{
	m_PhotonCount = PhotonCount;

	m_PhotonArray = &PhotonArray;

	//m_PhotonMapBoundingBox.MaxPositions = GIVector3( -GI_INFINITY, -GI_INFINITY, -GI_INFINITY );
	m_PhotonMapBoundingBox.MaxPositions = GIVector3( -GI_INFINITY,  -GI_INFINITY,  -GI_INFINITY );
	m_PhotonMapBoundingBox.MinPositions = GIVector3(  GI_INFINITY,  GI_INFINITY,  GI_INFINITY );

	// Build KD-Tree
	std::vector<unsigned int> PhotonIndexList;
	PhotonIndexList.reserve( m_PhotonCount );
	for( unsigned int i = 0; i < m_PhotonCount; i++ )
	{
		if( m_PhotonMapBoundingBox.MaxPositions.x < PhotonArray[i].Position.x )
			m_PhotonMapBoundingBox.MaxPositions.x = PhotonArray[i].Position.x;
		if( m_PhotonMapBoundingBox.MaxPositions.y < PhotonArray[i].Position.y )
			m_PhotonMapBoundingBox.MaxPositions.y = PhotonArray[i].Position.y;
		if( m_PhotonMapBoundingBox.MaxPositions.z < PhotonArray[i].Position.z )
			m_PhotonMapBoundingBox.MaxPositions.z = PhotonArray[i].Position.z;

		if( m_PhotonMapBoundingBox.MinPositions.x > PhotonArray[i].Position.x )
			m_PhotonMapBoundingBox.MinPositions.x = PhotonArray[i].Position.x;
		if( m_PhotonMapBoundingBox.MinPositions.y > PhotonArray[i].Position.y )
			m_PhotonMapBoundingBox.MinPositions.y = PhotonArray[i].Position.y;
		if( m_PhotonMapBoundingBox.MinPositions.z > PhotonArray[i].Position.z )
			m_PhotonMapBoundingBox.MinPositions.z = PhotonArray[i].Position.z;

		PhotonIndexList.push_back(i);
	}

	m_CurNodeSize = 0;
	m_RootNode = CreateNode( PhotonIndexList, m_PhotonCount, m_PhotonMapBoundingBox, m_PhotonMapBoundingBox.GetLongestAxis() );
	m_NodeSize = m_CurNodeSize;
}

GIPhotonMapKDTreeNode* GIPhotonMapKDTree::CreateNode( const std::vector<unsigned int> &PhotonIndexList, const int PhotonCount, const GIBoundingBox &BoundingBox, int SplitAxis )
{
	assert( PhotonCount > 0 );
	assert( 0 <= SplitAxis && SplitAxis < 3 );

	std::vector<unsigned int> LeftPhotonIndexList;
	std::vector<unsigned int> RightPhotonIndexList;
	LeftPhotonIndexList.reserve( PhotonCount );
	RightPhotonIndexList.reserve( PhotonCount );

	GIPhotonMapKDTreeNode *NewNode = new GIPhotonMapKDTreeNode;
	NewNode->NodeNum = m_CurNodeSize++;

	const float fPhotonCount = float(PhotonCount);

	float BoxMinPos = BoundingBox.MinPositions.array[SplitAxis];
	float BoxMaxPos = BoundingBox.MaxPositions.array[SplitAxis];
	float BoxLength = BoxMaxPos - BoxMinPos;

	bool bSplit = mMinimum_PhotonCount < PhotonCount && mMinimumNodeLength < BoxMaxPos - BoxMinPos;

	NewNode->LeftNode = NULL;
	NewNode->RightNode = NULL;
	NewNode->PhotonIndexArray = NULL;
	NewNode->PhotonCount = PhotonCount;

	if( bSplit )
	{
		NewNode->SplitAxis = SplitAxis;
		NewNode->bLeafNode = false;

		const float SplitPosition = BoxLength/2.0f + BoxMinPos;

		GIBoundingBox LeftBox = BoundingBox;
		GIBoundingBox RightBox = BoundingBox;
		LeftBox.MaxPositions.array[SplitAxis] = SplitPosition;
		RightBox.MinPositions.array[SplitAxis] = SplitPosition;

		NewNode->MinPosition = BoundingBox.MinPositions.array[SplitAxis];
		NewNode->MaxPosition = BoundingBox.MaxPositions.array[SplitAxis];
		NewNode->SplitPosition = SplitPosition;
		
		int leftOnly = 0;
		int rightOnly = 0;

		for( int i = 0; i < PhotonCount; i++ )
		{
			unsigned int PhotonIndex = PhotonIndexList[i];
			float PhotonPosition = (*m_PhotonArray)[PhotonIndex].Position.array[SplitAxis];

			bool leftFlag = false, rightFlag = false;
			if( PhotonPosition <= SplitPosition )
			{
				LeftPhotonIndexList.push_back( PhotonIndex );
				leftFlag = true;
			}
			// 경계에 있을 경우는 왼쪽에만 넣음
			if( SplitPosition < PhotonPosition )
			{
				RightPhotonIndexList.push_back( PhotonIndex );
				rightFlag = true;
			}

			if( leftFlag )
				leftOnly++;
			else if( rightFlag )
				rightOnly++;
			else
			{
				assert( false );
			}
		}

		unsigned int LeftListSize = LeftPhotonIndexList.size();
		unsigned int RightListSize = RightPhotonIndexList.size();

		//unsigned int *LeftPhotonIndexArray = new unsigned int[SAHCost.LeftCount + SAHCost.PlanarCount];
		//unsigned int *RightPhotonIndexArray = new unsigned int[SAHCost.RightCount];

		int LeftCount = LeftListSize;
		int RightCount = RightListSize;
		assert( 0 <= LeftCount && LeftCount <= PhotonCount );
		assert( 0 <= RightCount && RightCount <= PhotonCount );
		assert( LeftCount + RightCount >= PhotonCount );

		int LeftOnlyCount = PhotonCount - RightCount;
		assert( 0 <= LeftOnlyCount && LeftOnlyCount <= PhotonCount );

		int nextSplitAxis = (++SplitAxis)%3;
		if( LeftCount > 0 )
			NewNode->LeftNode = CreateNode( LeftPhotonIndexList, LeftCount, LeftBox, nextSplitAxis );
		if( RightCount > 0 )
			NewNode->RightNode = CreateNode( RightPhotonIndexList, RightCount, RightBox, nextSplitAxis );

		//delete[] LeftPhotonIndexArray;
		//delete[] RightPhotonIndexArray;
	}
	else
	{
		NewNode->SplitAxis = -1;
		NewNode->bLeafNode = true;
		//NewNode->MaxPosition = BoundingBox.MinPositions.array
		
		NewNode->PhotonIndexArray = new unsigned int[PhotonCount];
		for( int i = 0; i < PhotonCount; i++ )
			NewNode->PhotonIndexArray[i] = PhotonIndexList[i];
	}

	assert( NewNode->IsValid() );
	return NewNode;
}