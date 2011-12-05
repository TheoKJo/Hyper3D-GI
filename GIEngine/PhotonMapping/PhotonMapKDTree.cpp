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
	: mMinimumPhotonCount(5), mMinimumNodeLength(0), mCurNodePhotonIndex(0), mCurNodeSize(0), 
	mRootNode(NULL), mNodeSize(0), mPhotonCount(0), mNodeArray(NULL), mPhotonArray(NULL)
{
	
	
}

GIPhotonMapKDTree::~GIPhotonMapKDTree()
{
	mRootNode = NULL;
	SafeDeleteArray( &mNodeArray );
	mPhotonArray = NULL;
}

bool GIPhotonMapKDTree::IsBuilt()
{
	return mPhotonCount > 0 && mPhotonArray != NULL;
}

void GIPhotonMapKDTree::SetBuildParameters( int MinimumPhotonCount, float MinimumNodeLength )
{
	mMinimumPhotonCount = MinimumPhotonCount;
	mMinimumNodeLength = MinimumNodeLength;
}

void GIPhotonMapKDTree::ConstructKDTree( const std::vector<GIPhoton> &PhotonArray, unsigned int PhotonCount )
{
	mPhotonCount = PhotonCount;

	mPhotonArray = &PhotonArray;

	//mPhotonMapBoundingBox.MaxPositions = GIVector3( -GI_INFINITY, -GI_INFINITY, -GI_INFINITY );
	mPhotonMapBoundingBox.MaxPositions = GIVector3( -GI_INFINITY,  -GI_INFINITY,  -GI_INFINITY );
	mPhotonMapBoundingBox.MinPositions = GIVector3(  GI_INFINITY,  GI_INFINITY,  GI_INFINITY );

	// Build KD-Tree
	std::vector<unsigned int> PhotonIndexList;
	PhotonIndexList.reserve( mPhotonCount );
	for( unsigned int i = 0; i < mPhotonCount; i++ )
	{
		if( mPhotonMapBoundingBox.MaxPositions.x < PhotonArray[i].Position.x )
			mPhotonMapBoundingBox.MaxPositions.x = PhotonArray[i].Position.x;
		if( mPhotonMapBoundingBox.MaxPositions.y < PhotonArray[i].Position.y )
			mPhotonMapBoundingBox.MaxPositions.y = PhotonArray[i].Position.y;
		if( mPhotonMapBoundingBox.MaxPositions.z < PhotonArray[i].Position.z )
			mPhotonMapBoundingBox.MaxPositions.z = PhotonArray[i].Position.z;

		if( mPhotonMapBoundingBox.MinPositions.x > PhotonArray[i].Position.x )
			mPhotonMapBoundingBox.MinPositions.x = PhotonArray[i].Position.x;
		if( mPhotonMapBoundingBox.MinPositions.y > PhotonArray[i].Position.y )
			mPhotonMapBoundingBox.MinPositions.y = PhotonArray[i].Position.y;
		if( mPhotonMapBoundingBox.MinPositions.z > PhotonArray[i].Position.z )
			mPhotonMapBoundingBox.MinPositions.z = PhotonArray[i].Position.z;

		PhotonIndexList.push_back(i);
	}

	mCurNodeSize = 0;
	mRootNode = CreateNode( PhotonIndexList, mPhotonCount, mPhotonMapBoundingBox, mPhotonMapBoundingBox.GetLongestAxis() );
	mNodeSize = mCurNodeSize;
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
	NewNode->NodeNum = mCurNodeSize++;

	const float fPhotonCount = float(PhotonCount);

	float BoxMinPos = BoundingBox.MinPositions.array[SplitAxis];
	float BoxMaxPos = BoundingBox.MaxPositions.array[SplitAxis];
	float BoxLength = BoxMaxPos - BoxMinPos;

	bool bSplit = mMinimumPhotonCount < PhotonCount && mMinimumNodeLength < BoxMaxPos - BoxMinPos;

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
			float PhotonPosition = (*mPhotonArray)[PhotonIndex].Position.array[SplitAxis];

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