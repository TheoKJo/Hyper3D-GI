/*!
 * \file		KDTreeStructure.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include "KDTreeStructure.h"

#include <vector>
#include <assert.h>

#include <algorithm>
#include <fstream>
#include <stack>

KDTreeStructure::KDTreeStructure()
	: m_RootNode(NULL), m_NodeSize(0), m_NodeArray(NULL), 
	// For Build
	m_TriangleCount(0), m_TriangleBoxArray(NULL), 
	m_CurNodeTriangleIndex(0),  m_CurNodeSize(0)
{
	m_IntersectionCost = 20.0f;
	mTraversalCost = 1.5f;
	m_MinimumTriangleCount = 10;
}

KDTreeStructure::~KDTreeStructure()
{
	DestructTree();
}

void KDTreeStructure::ConstructKDTree( const GITriangle *TriangleList, const int TriangleCount, const GIBoundingBox &BoundingBox )
{
	m_TriangleCount = TriangleCount;

	unsigned int *TriangleArray = new unsigned int[TriangleCount];
	for( int i = 0; i < 3; i++ )
	{
		m_TriangleBoxArray = new TriangleBox[TriangleCount];
	}

// Initialize Edges
	for( int i = 0; i < TriangleCount; i++ )
	{
		TriangleArray[i] = i;
		m_TriangleBoxArray[i].Initialize( &TriangleList[i] );
		//TriangleToBoundingBox( BoxMin, BoxMax, &TriangleList[i] );
	}

// Build KD-Tree
	std::vector<unsigned int> TriangleList;
	TriangleList.reserve( TriangleCount );
	for( int i = 0; i < TriangleCount; i++ )
		TriangleList.push_back(i);

	m_CurNodeSize = 0;
	m_RootNode = CreateNode( 0, TriangleList, TriangleCount, BoundingBox );
	m_NodeSize = m_CurNodeSize;
	m_SceneBoundingBox = BoundingBox;

	delete[] TriangleArray;
}

void KDTreeStructure::DestructTree()
{
}

bool KDTreeStructure::IsBuilt()
{
	return m_RootNode != NULL;
}

void KDTreeStructure::SetBuildParameters( const float &IntersectionCost, const float &TraversalCost, const int Minimum_TriangleCount )
{
	m_IntersectionCost = IntersectionCost;
	mTraversalCost = TraversalCost;
	m_MinimumTriangleCount = Minimum_TriangleCount;
}

#define BEGIN_KDTREE_HEADER "BEGIN_KD-TREE_HEADER" 
#define END_KDTREE_HEADER "END_KD-TREE_HEADER" 

bool KDTreeStructure::LoadFromFile( const char *Filename )
{
	if( IsBuilt() )
		DestructTree();
	std::fstream fs;
	fs.open( Filename, std::ios::in );
	if( !fs )
		return false;

	char name[255];

	fs >> name;
	if( strcmp( name, BEGIN_KDTREE_HEADER ) == 0 )
	{
		fs >> name;
		while( strcmp( name, END_KDTREE_HEADER ) != 0 )
		{
			if( strcmp( name, "NodeSize" ) == 0 )
			{
				fs >> m_NodeSize;
			}
			else if( strcmp( name, "SceneBoundaryBox" ) == 0 )
			{
				fs >> m_SceneBoundingBox.MinPositions.x; fs >> m_SceneBoundingBox.MinPositions.y; fs >> m_SceneBoundingBox.MinPositions.z;
				fs >> m_SceneBoundingBox.MaxPositions.x; fs >> m_SceneBoundingBox.MaxPositions.y; fs >> m_SceneBoundingBox.MaxPositions.z;
			}
			else if( strcmp( name, "%" ) == 0 )
			{
				fs.getline( name, 255 );
			}
			fs >> name;
		}
	}
	else
		return false;

	if( m_NodeSize <= 0 )
		return false;

	m_NodeArray = new RtKDTreeNode[m_NodeSize];
	for( int i = 0; i < m_NodeSize; i++ )
	{
		fs >> m_NodeArray[i].NodeNum;
		int NodeType;
		fs >> NodeType;
		if( NodeType < 0 || 3 < NodeType )
			return false;

		m_NodeArray[i].NodeNum = i;
		m_NodeArray[i].bLeafNode = NodeType == 0;

		if( NodeType == 0 )
		{
			fs >> m_NodeArray[i].TriangleCount;

			if( m_NodeArray[i].TriangleCount != 0 )
			{
				char ch;
				fs >> ch;
				if( ch != '[' )
					return false;

				m_NodeArray[i].TriangleIndexArray = new unsigned int[m_NodeArray[i].TriangleCount];

				for( int j = 0; j < m_NodeArray[i].TriangleCount; j++ )
					fs >> m_NodeArray[i].TriangleIndexArray[j];

				fs >> ch;
				if( ch != ']' )
					return false;
			}
		}
		else
		{
			fs >> m_NodeArray[i].TriangleCount;
			fs >> m_NodeArray[i].SplitAxis;
			fs >> m_NodeArray[i].MinPosition;
			fs >> m_NodeArray[i].SplitPosition;
			fs >> m_NodeArray[i].MaxPosition;

			char ch;
			fs >> ch;
			if( ch != '[' )
				return false;

			if( NodeType & 1 )
			{
				int LeftIndex;
				fs >> LeftIndex;
				m_NodeArray[i].LeftNode = &m_NodeArray[LeftIndex];
			}
			if( NodeType & 2 )
			{
				int RightIndex;
				fs >> RightIndex;
				m_NodeArray[i].RightNode = &m_NodeArray[RightIndex];
			}

			fs >> ch;
			if( ch != ']' )
				return false;
		}
	}
	m_RootNode = &m_NodeArray[0];
	return true;
}

bool KDTreeStructure::SaveToFile( const char *Filename )
{
	if( IsBuilt() == false )
		return false;

	std::fstream fs;
	fs.open( Filename, std::ios::out );
	if( !fs )
		return false;

	fs << BEGIN_KDTREE_HEADER << std::endl;
		fs << "NodeSize"  << '\t' << m_NodeSize << std::endl;
		fs << "SceneBoundaryBox"  << '\t' << 
			m_SceneBoundingBox.MinPositions.x << '\t' << m_SceneBoundingBox.MinPositions.y << '\t' << m_SceneBoundingBox.MinPositions.z << '\t' <<
			m_SceneBoundingBox.MaxPositions.x << '\t' << m_SceneBoundingBox.MaxPositions.y << '\t' << m_SceneBoundingBox.MaxPositions.z << std::endl;
		// Scene Name, KD-Tree Name

		// Comments
		//fs << "% For Internal Node\n% NodeType\tTriangleCount\tSplitAxis\tMinPosition\tMaxPosition" << std::endl;
		fs << "% For Internal Node\n% NodeType\tTriangleCount\tSplitAxis\tMinPosition\tSplitPosition\tMaxPosition" << std::endl;
		fs << "% For Leaf Node\n% NodeType\tTriangleCount" << std::endl;
	fs << END_KDTREE_HEADER << std::endl << std::endl;

	std::stack<RtKDTreeNode*> Stack;
	Stack.push( GetRootNode() );

	while( Stack.empty() == false )
	{
		RtKDTreeNode *curNode = Stack.top();
		Stack.pop();

		assert( curNode != NULL );

		fs << curNode->NodeNum << '\t';

		if( curNode->IsLeafNode() )
		{
			fs << '0' << '\t'; //!< Node Type - 0: Leaf, 1: Left Only, 2: Right Only, 3: Both
			fs << curNode->TriangleCount << '\t';
				//<<
				//curNode->SplitAxis << '\t' << 
				//curNode->MinPosition << '\t' << 
				//curNode->MaxPosition;
			if( curNode->TriangleCount != 0 )
			{
				fs << '\t' << '[';
				for( int i = 0;;)
				{
					fs << curNode->TriangleIndexArray[i];
					i++;
					if( i == curNode->TriangleCount )
						break;
					fs << ' ';
				}
				fs << ']';
			}
			fs << std::endl;
		}
		else
		{
			//! Node Type - 0: Leaf, 1: Left Only, 2: Right Only, 3: Both
			int NodeType = 0;
			if( curNode->LeftNode != NULL )
				NodeType += 1;
			if( curNode->RightNode != NULL )
				NodeType += 2;
			assert( 1 <= NodeType && NodeType <= 3 );				

			fs << NodeType << '\t';
			fs << curNode->TriangleCount << '\t' <<
				curNode->SplitAxis << '\t' << 
				curNode->MinPosition << '\t' << 
				curNode->SplitPosition << '\t' << 
				curNode->MaxPosition << '\t';

			fs << '[';
			if( curNode->LeftNode != NULL )
				fs << curNode->LeftNode->NodeNum;
			if( NodeType == 3 )
				fs << ' ';
			if( curNode->RightNode != NULL )
				fs << curNode->RightNode->NodeNum;
			fs << ']';

			if( curNode->RightNode != NULL )
				Stack.push( curNode->RightNode );
			if( curNode->LeftNode != NULL )
				Stack.push( curNode->LeftNode );
			fs << std::endl;
		}
	}
	return true;
}

RtKDTreeNode* KDTreeStructure::CreateNode( int StartEdgeIndex, const std::vector<unsigned int> &TriangleList, const int TriangleCount, const GIBoundingBox &BoundingBox )
{
	assert( TriangleCount > 0 );

	std::vector<unsigned int> PlanarIndexList;
	std::vector<unsigned int> LeftTriangleList;
	std::vector<unsigned int> RightTriangleList;
	PlanarIndexList.reserve( TriangleCount );
	LeftTriangleList.reserve( TriangleCount );
	RightTriangleList.reserve( TriangleCount );

	RtKDTreeNode *NewNode = new RtKDTreeNode;
	NewNode->NodeNum = m_CurNodeSize++;

	RtSAHCost SAHCost;
	//ResetInvalid();

	const float fTriangleCount = float(TriangleCount);

	const float NonSplitSAHCost = fTriangleCount * m_IntersectionCost;
	SAHCost.SplitCost = NonSplitSAHCost;

	if( m_MinimumTriangleCount <= TriangleCount )
	{
		for( int Axis = 0; Axis < 3; Axis++ )
			EvaluateCost( SAHCost, TriangleList, TriangleCount, BoundingBox, Axis );
	}

	NewNode->LeftNode = NULL;
	NewNode->RightNode = NULL;
	NewNode->TriangleIndexArray = NULL;
	NewNode->TriangleCount = TriangleCount;

	if( SAHCost.IsSplit )
	{
		NewNode->SplitAxis = SAHCost.Axis;
		NewNode->bLeafNode = false;	

		const int Axis = SAHCost.Axis;
		const float SplitPosition = SAHCost.SplitPosition;

		GIBoundingBox LeftBox = BoundingBox;
		GIBoundingBox RightBox = BoundingBox;
		LeftBox.MaxPositions.array[SAHCost.Axis] = SplitPosition;
		RightBox.MinPositions.array[SAHCost.Axis] = SplitPosition;

		NewNode->MinPosition = BoundingBox.MinPositions.array[SAHCost.Axis];
		NewNode->MaxPosition = BoundingBox.MaxPositions.array[SAHCost.Axis];
		NewNode->SplitPosition = SplitPosition;
		
		int leftOnly = 0;
		int rightOnly = 0;
		int both = 0;

		for( int i = 0; i < TriangleCount; i++ )
		{
			unsigned int TriangleIndex = TriangleList[i];

			const float &LeftEdgePosition = m_TriangleBoxArray[TriangleIndex].m_BoxMin.array[Axis];
			const float &RightEdgePosition = m_TriangleBoxArray[TriangleIndex].m_BoxMax.array[Axis];
			const bool isPlanar = LeftEdgePosition == SplitPosition && LeftEdgePosition == RightEdgePosition;

			if( isPlanar )
				PlanarIndexList.push_back( TriangleIndex );
			else
			{
				bool leftFlag = false, rightFlag = false, bothFlag = false;
				if( LeftEdgePosition < SplitPosition )
				{
					LeftTriangleList.push_back( TriangleIndex );
					leftFlag = true;
				}
				if( SplitPosition < RightEdgePosition )
				{
					RightTriangleList.push_back( TriangleIndex );
					rightFlag = true;
				}
				bothFlag = leftFlag && rightFlag;

				if( bothFlag )
					both++;
				else
				{
					if( leftFlag )
						leftOnly++;
					else if( rightFlag )
						rightOnly++;
					else
					{
						assert( false );
					}
				}				
			}
			//else
			{
				/*LeftTriangleList.push_back( TriangleIndex );
				RightTriangleList.push_back( TriangleIndex );*/
			}
		}

		assert( SAHCost.CrossTriangleCount == both );
		assert( leftOnly == SAHCost.LeftOnlyCount );
		assert( rightOnly == SAHCost.RightOnlyCount );


		unsigned int LeftListSize = LeftTriangleList.size();
		unsigned int RightListSize = RightTriangleList.size();
		assert( LeftTriangleList.size() == SAHCost.LeftCount );
		assert( RightTriangleList.size() == SAHCost.RightCount );

		//unsigned int *LeftTriangleIndexArray = new unsigned int[SAHCost.LeftCount + SAHCost.PlanarCount];
		//unsigned int *RightTriangleIndexArray = new unsigned int[SAHCost.RightCount];

		int LeftCount = LeftListSize + SAHCost.PlanarCount;
		int RightCount = RightListSize;
		assert( 0 <= LeftCount && LeftCount <= TriangleCount );
		assert( 0 <= RightCount && RightCount <= TriangleCount );
		assert( LeftCount + RightCount >= TriangleCount );

		int LeftOnlyCount = TriangleCount - RightCount;
		assert( 0 <= LeftOnlyCount && LeftOnlyCount <= TriangleCount );

		for( unsigned int i = 0; i < PlanarIndexList.size(); i++ )
			LeftTriangleList.push_back( PlanarIndexList[i] );

		if( LeftCount > 0 )
			NewNode->LeftNode = CreateNode( 0, LeftTriangleList, LeftCount, LeftBox );
		if( SAHCost.RightCount > 0 )
			NewNode->RightNode = CreateNode( 0, RightTriangleList, RightCount, RightBox );

		//delete[] LeftTriangleIndexArray;
		//delete[] RightTriangleIndexArray;
	}
	else
	{
		NewNode->SplitAxis = -1;
		NewNode->bLeafNode = true;
		//NewNode->MaxPosition = BoundingBox.MinPositions.array
		
		NewNode->TriangleIndexArray = new unsigned int[TriangleCount];
		for( int i = 0; i < TriangleCount; i++ )
			NewNode->TriangleIndexArray[i] = TriangleList[i];
	}

	assert( NewNode->IsValid() );
	return NewNode;
}

void KDTreeStructure::EvaluateCost( RtSAHCost &SAHCost, const std::vector<unsigned int> &TriangleList, const int TriangleCount, const GIBoundingBox &BoundingBox, int Axis )
{
	const int Axis0 = Axis;
	const int Axis1 = (Axis + 1)%3 ;
	const int Axis2 = (Axis + 2)%3;

	GIVector3 DifferentVector = BoundingBox.MaxPositions - BoundingBox.MinPositions;
	assert( 0.0f < DifferentVector.array[0] ); assert( 0.0f < DifferentVector.array[1] ); assert( 0.0f < DifferentVector.array[2] );

	float sidePlane = DifferentVector.array[Axis1] * DifferentVector.array[Axis2];
	float extent = DifferentVector.array[Axis1] + DifferentVector.array[Axis2];

	const float BoxMinPositionAxis0 = BoundingBox.MinPositions.array[Axis0];
	const float BoxMaxPositionAxis0 = BoundingBox.MaxPositions.array[Axis0];

	// note: half size
	float parentArea = DifferentVector.array[Axis0] * extent + sidePlane;
	assert( 0.0f < parentArea );

	int leftTriangleCount = 0;
	int rightTriangleCount = TriangleCount;

	RtEdge *EdgeArray = new RtEdge[TriangleCount*2];

	int EdgeSize = InitializeEdge( EdgeArray, TriangleList, TriangleCount, Axis );
	int EdgeIndex = 0;

	/*while( EdgeIndex < EdgeSize && BoxMinPositionAxis0 == EdgeArray[EdgeIndex].position )
		EdgeIndex++;*/

	while( EdgeIndex < EdgeSize && EdgeArray[EdgeIndex].position < BoxMaxPositionAxis0 )
	{
		int SplitEdgeIndex = EdgeIndex;
		RtEdge &curEdge = EdgeArray[EdgeIndex];
		float curPosition = curEdge.position;

		int local_open = 0;
		int local_close = 0;
		int local_planar = 0;

		do
		{
			if( curEdge.isPlanar() )
			{
				if( curEdge.isLeft() )
					local_planar++;
			}
			else
			{
				if( curEdge.isLeft() )
					local_open++;
				else
					local_close++;
			}
			EdgeIndex++;
			if( EdgeIndex == EdgeSize )
				break;
			assert( EdgeIndex < EdgeSize );
			curEdge = EdgeArray[EdgeIndex];
		} while( curPosition == curEdge.position );
		//assert( BoxMinPositionAxis0 <= curPosition && curPosition <= BoxMaxPositionAxis0 );

		if( curPosition <= BoxMinPositionAxis0 )
		{
			rightTriangleCount -= local_planar;
			rightTriangleCount -= local_close;
			leftTriangleCount += local_planar;
			leftTriangleCount += local_open;
			continue;
		}

		float leftBoxArea = (curPosition - BoxMinPositionAxis0) * extent + sidePlane;
		float rightBoxArea = (BoxMaxPositionAxis0 - curPosition) * extent + sidePlane;

		float leftProbability = leftBoxArea/parentArea;
		float rightProbability = rightBoxArea/parentArea;

		assert( 0.0f <= leftProbability && leftProbability <= 1.0f );
		assert( 0.0f <= rightProbability && rightProbability <= 1.0f );

		int planarCount = local_planar; //!< planar 는 local only

		rightTriangleCount -= planarCount;
		rightTriangleCount -= local_close;

		// 세개가 기본
		/*!
		 * leftTriangleCount
		 * rightTriangleCount
		 * planarCount
		 */

#define ASSERtRANGE( value, min, max ) {assert( min <= value && value <= max );}

		// 디버깅용
		int leftOnlyCount = TriangleCount - rightTriangleCount - planarCount;
		int rightOnlyCount = TriangleCount - leftTriangleCount - planarCount;
		int crossTriangleCount = leftTriangleCount + rightTriangleCount - (TriangleCount-planarCount);

		ASSERtRANGE( leftOnlyCount, 0, TriangleCount );
		ASSERtRANGE( rightOnlyCount, 0, TriangleCount );
		ASSERtRANGE( crossTriangleCount, 0, TriangleCount );

		// Plane 는 무조건 왼쪽으로 가정.
		// TODO: [2] 로 두번 계산해서 비교해야함.
		float SAH = mTraversalCost + m_IntersectionCost * float(leftTriangleCount) * leftProbability + m_IntersectionCost * float(rightTriangleCount) * rightProbability;
		if( SAH < SAHCost.SplitCost )
		{
			SAHCost.IsSplit = true;
			SAHCost.SplitCost = SAH;
			SAHCost.Axis = Axis0;
			
			SAHCost.SplitEdgeIndex = SplitEdgeIndex;
			SAHCost.SplitPosition = curPosition;

			SAHCost.LeftCount = leftTriangleCount;	//!< planar 제외, cross 포함
			SAHCost.RightCount = rightTriangleCount; //!< planar 제외, cross 포함

			SAHCost.LeftOnlyCount = leftOnlyCount;
			SAHCost.RightOnlyCount = rightOnlyCount;
			SAHCost.CrossTriangleCount = crossTriangleCount; //!< planar 제외

			SAHCost.PlanarCount = planarCount;

			// EdgeIndex 는 RightBox 의 firat Index
			ASSERtRANGE( SAHCost.SplitEdgeIndex, 0, EdgeSize-1 );
		}

		leftTriangleCount += planarCount;
		leftTriangleCount += local_open;
	}
	delete[] EdgeArray;
}

int KDTreeStructure::InitializeEdge( RtEdge *EdgeArray, const std::vector<unsigned int> &TriangleList, const int TriangleCount, int Axis )
{
	int EdgeIndex = 0;
	for( int i = 0; i < TriangleCount; i++ )
	{
		assert( EdgeIndex < TriangleCount*2 );
		unsigned int TriangleIndex = TriangleList[i];
		
		bool bPlanar = m_TriangleBoxArray[TriangleIndex].isPlanar( Axis );
		if( bPlanar )
		{
			assert( m_TriangleBoxArray[TriangleIndex].m_BoxMin.array[Axis] == m_TriangleBoxArray[TriangleIndex].m_BoxMax.array[Axis] );

			EdgeArray[EdgeIndex].axis = Axis;
			EdgeArray[EdgeIndex].TriangleIndex = TriangleIndex;
			EdgeArray[EdgeIndex].setPlanar();
			EdgeArray[EdgeIndex].setLeft();
			EdgeArray[EdgeIndex].position = m_TriangleBoxArray[TriangleIndex].m_BoxMin.array[Axis];
			EdgeIndex++;
		}
		else
		{
			assert( m_TriangleBoxArray[TriangleIndex].m_BoxMin.array[Axis] != m_TriangleBoxArray[TriangleIndex].m_BoxMax.array[Axis] );

			EdgeArray[EdgeIndex].axis = Axis;
			EdgeArray[EdgeIndex].TriangleIndex = TriangleIndex;
			EdgeArray[EdgeIndex].setPlanar( false );
			EdgeArray[EdgeIndex].setLeft();
			EdgeArray[EdgeIndex].position = m_TriangleBoxArray[TriangleIndex].m_BoxMin.array[Axis];
			EdgeIndex++;

			EdgeArray[EdgeIndex].axis = Axis;
			EdgeArray[EdgeIndex].TriangleIndex = TriangleIndex;
			EdgeArray[EdgeIndex].setPlanar( false );
			EdgeArray[EdgeIndex].setRight();
			EdgeArray[EdgeIndex].position = m_TriangleBoxArray[TriangleIndex].m_BoxMax.array[Axis];
			EdgeIndex++;
		}
	}
	SortEdgeArray( EdgeArray, EdgeIndex );
	return EdgeIndex;
}

static int size = 0;

inline bool LessThan( RtEdge &Edge0, RtEdge &Edge1 )
{
	return Edge0.position < Edge1.position;
}

inline void Swap( RtEdge &Edge0, RtEdge &Edge1 )
{
	RtEdge temp = Edge0;
	Edge0 = Edge1;
	Edge1 = temp;
}

void QuickSort( RtEdge *EdgeArray, int left, int right )
{
	if( right - left + 1 < 2 )
		return;
	else if( right - left + 1 == 2 )
	{
		if( EdgeArray[left].position > EdgeArray[right].position )
			Swap( EdgeArray[left], EdgeArray[right] );
		return;
	}

	int left_hold = left;
	int right_hold = right;

	// select pivot
	int middle = (left+right)/2;
	int pivot = right;
	float pivotValue = EdgeArray[middle].position;
	Swap( EdgeArray[middle], EdgeArray[right] );
	right--;

	assert( 0 <= left && left < right_hold );
	assert( 0 <= right && right < right_hold );

	while( right - left + 1 > 1 )
	{
		// 같은건 왼쪽으로
		while( left < pivot && EdgeArray[left].position <= pivotValue )
			left++;
		while( right >= 0 && pivotValue < EdgeArray[right].position )
			right--;

		// 규칙에 안맞는 원소들을 바꿔야 할 경우.
		if( right - left + 1 >= 2 && right >= 0 && left < pivot )
		{
			//cout << "Swap : left(" << left << ") right(" << right << ")" << endl;
			assert( EdgeArray[left].position >= EdgeArray[right].position );
			Swap( EdgeArray[left], EdgeArray[right] );
			assert( EdgeArray[left].position <= pivotValue );
			assert( pivotValue < EdgeArray[right].position );
		}
	}
	right++;

	if( right != pivot )
	{
		assert( EdgeArray[right].position > pivotValue );
		// right 는 규칙에 맞는 마지막 원소를 가르키고 있음.
		Swap( EdgeArray[right], EdgeArray[pivot] );
	}
	pivot = right;

#ifdef DEBUG
	for( int i = left_hold; i < pivot; i++ ) 
		assert( EdgeArray[i].position <= pivotValue );
	for( int i = pivot+1; i <= right_hold; i++ )
		assert( pivotValue < EdgeArray[i].position );
#endif

	QuickSort( EdgeArray, left_hold, pivot-1 );
	QuickSort( EdgeArray, pivot+1, right_hold );
}

struct RANGE
{
	int left;
	int right;
};

void QuickSortWithStack( RtEdge *EdgeArray, int left, int right )
{
	std::stack<RANGE> Stack;
	RANGE r = {left, right };
	Stack.push( r );

	while( Stack.empty() == false )
	{
		/*if( Stack.empty() )
			return;*/
		RANGE r = Stack.top();
		left = r.left;
		right = r.right;
		Stack.pop();

		if( right - left + 1 < 2 )
			continue;
		else if( right - left + 1 == 2 )
		{
			if( EdgeArray[left].position > EdgeArray[right].position )
				Swap( EdgeArray[left], EdgeArray[right] );
			continue;
		}

		int left_hold = left;
		int right_hold = right;

		// select pivot
		int middle = (left+right)/2;
		int pivot = right;
		float pivotValue = EdgeArray[middle].position;
		Swap( EdgeArray[middle], EdgeArray[right] );
		right--;

		assert( 0 <= left && left < right_hold );
		assert( 0 <= right && right < right_hold );

		while( right - left + 1 > 1 )
		{
			// 같은건 왼쪽으로
			while( left < pivot && EdgeArray[left].position <= pivotValue )
				left++;
			while( right >= 0 && pivotValue < EdgeArray[right].position )
				right--;

			// 규칙에 안맞는 원소들을 바꿔야 할 경우.
			if( right - left + 1 >= 2 && right >= 0 && left < pivot )
			{
				//cout << "Swap : left(" << left << ") right(" << right << ")" << endl;
				assert( EdgeArray[left].position >= EdgeArray[right].position );
				Swap( EdgeArray[left], EdgeArray[right] );
				assert( EdgeArray[left].position <= pivotValue );
				assert( pivotValue < EdgeArray[right].position );
			}
		}
		right++;

		if( right != pivot )
		{
			assert( EdgeArray[right].position > pivotValue );
			// right 는 규칙에 맞는 마지막 원소를 가르키고 있음.
			Swap( EdgeArray[right], EdgeArray[pivot] );
		}
		pivot = right;

	#ifdef DEBUG
		for( int i = left_hold; i < pivot; i++ ) 
			assert( EdgeArray[i].position <= pivotValue );
		for( int i = pivot+1; i <= right_hold; i++ )
			assert( pivotValue < EdgeArray[i].position );
	#endif

		RANGE leftRange = { left_hold, pivot-1 };
		RANGE rightRange = { pivot+1, right_hold };

		Stack.push( rightRange );
		Stack.push( leftRange );

		//QuickSort( EdgeArray, left_hold, pivot-1 );
		//QuickSort( EdgeArray, pivot+1, right_hold );
	}
}

void SelectionSort( RtEdge *EdgeArray, int ArraySize )
{
	//! selection sort
	for( int i = 0; i < ArraySize-1; i++ )
	{
		float pivotValue = EdgeArray[i].position;
		for( int j = i+1; j < ArraySize; j++ )
		{
			if( EdgeArray[j].position < pivotValue )
			{
				// swap
				RtEdge temp = EdgeArray[i];
				EdgeArray[i] = EdgeArray[j];
				EdgeArray[j] = temp;
				pivotValue = EdgeArray[i].position;
			}
		}
	}
}

void KDTreeStructure::SortEdgeArray( RtEdge *EdgeArray, int EdgeSize )
{
	size = EdgeSize;
	//QuickSort( EdgeArray, 0, EdgeSize-1 );
	QuickSortWithStack( EdgeArray, 0, EdgeSize-1 );
	//SelectionSort( EdgeArray, EdgeSize );
	for( int i = 0; i < EdgeSize-1; i++ )
		assert( EdgeArray[i].position <= EdgeArray[i+1].position );
}

void KDTreeStructure::TriangleToBoundingBox( GIVector3 &BoxMin, GIVector3 &BoxMax, const GITriangle *Triangle )
{
	BoxMin = GIVector3( 1e15f, 1e15f, 1e15f );
	BoxMax = GIVector3( -1e15f, -1e15f, -1e15f );

	for( int i = 0; i < 3; i++ )
	{
		if( BoxMin.array[i] > Triangle->vg0.Vertex.array[i] )
			BoxMin.array[i] = Triangle->vg0.Vertex.array[i];
		if( BoxMin.array[i] > Triangle->vg1.Vertex.array[i] )
			BoxMin.array[i] = Triangle->vg1.Vertex.array[i];
		if( BoxMin.array[i] > Triangle->vg2.Vertex.array[i] )
			BoxMin.array[i] = Triangle->vg2.Vertex.array[i];

		if( BoxMax.array[i] < Triangle->vg0.Vertex.array[i] )
			BoxMax.array[i] = Triangle->vg0.Vertex.array[i];
		if( BoxMax.array[i] < Triangle->vg1.Vertex.array[i] )
			BoxMax.array[i] = Triangle->vg1.Vertex.array[i];
		if( BoxMax.array[i] < Triangle->vg2.Vertex.array[i] )
			BoxMax.array[i] = Triangle->vg2.Vertex.array[i];
	}
}
