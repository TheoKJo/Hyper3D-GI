/*!
 * \file		Raytracer.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include "Raytracer.h"

#include <Utility.h>
#include <RtScene.h>

// inline
#include "Intersect.h"

#include "KDTreeStructure.h"

#include <math.h>

#include <stack>

using namespace GIEngine;

KDTreeStructure* Raytracer::BuildKDTree( RtScene *pScene, const char *strStructureFilename, bool bLoadKDTree/* = true*/ )
{
	KDTreeStructure* KDTree = new KDTreeStructure();

	KDTree->m_IntersectionCost = 20.0f;
	KDTree->mTraversalCost = 1.5f;

	bool Loaded = false;
	if( bLoadKDTree )
		Loaded = KDTree->LoadFromFile( strStructureFilename );
	if( Loaded == false )
	{
		KDTree->ConstructKDTree( pScene->GetTriangleList(), pScene->GetTriangleCount(), pScene->GetBoundingBox() );
		KDTree->SaveToFile( strStructureFilename );
		RtKDTreeNode *RootNode = KDTree->GetRootNode();
	}
	if( KDTree->IsBuilt() == false )
	{
		delete KDTree;
		KDTree = NULL;
	}
	return KDTree;
}

GIHit Raytracer::ShootRay( RtScene *rtScene, KDTreeStructure *KDTree, const GIRay &Ray )
{
	assert( KDTree != NULL && KDTree->IsBuilt() );

	// TODO : Boundary?, const Stack!!!! 가급적 모든걸 global 로!!! (각 thread 에 대해 가지고 있도록!)

	//_CrtMemState s1, s2, difference;
	//_CrtMemCheckpoint( &s1 );

	GIHit Hit;
	Hit.dist = Ray.FarDistance;

	/*int MajorAxis = 0;
	if( fabs( Ray.dir[0] ) < fabs( Ray.dir[1] ) )
		MajorAxis = 1;
	if( fabs( Ray.dir[MajorAxis] ) < fabs( Ray.dir[2] ) )
		MajorAxis = 2;

	assert( Ray.dir[MajorAxis] != 0.0f );
	if( Ray.dir[MajorAxis] < 0.0f )
		MajorAxis *= -1;*/

	// TODO: Both Check 도 해야할듯?
	// Ray.dir[i] 가 0.0f 일 경우에 대한 대처는...

	enum TRAVERSAL_FLAG {
		LEFT_FIRST,
		RIGHT_FIRST, 
		BOTH
	};
	TRAVERSAL_FLAG TraversalFlag[3];
	for( int i = 0; i < 3; i++ )
	{
		TraversalFlag[i] = Ray.dir[i]>0.0f?LEFT_FIRST:(Ray.dir[i]<0.0f)?RIGHT_FIRST:BOTH;
	}

	// TODO: 속도 문제가 좀 있을 수 있음.
	struct StackStruct {
		RtKDTreeNode *pNode;
		GIBoundingBox BoundingBox;
	};

	StackStruct RootStatkNode = { KDTree->GetRootNode(), KDTree->GetBoundingBox() };

	std::vector<StackStruct> StackStorage;
	// TODO: 500?? 절대 안됨!!
	StackStorage.reserve( 500 );
	std::stack<StackStruct, std::vector<StackStruct> > Stack( StackStorage );
	Stack.push( RootStatkNode );

	// assume Ray hit pNode

	static int modulo[5] = { 0, 1, 2, 0, 1 };

	while( Stack.empty() == false )
	{
		StackStruct CurStatkNode = Stack.top();
		RtKDTreeNode *curNode = CurStatkNode.pNode;
		Stack.pop();

		assert( curNode != NULL );

		if( curNode->IsLeafNode() )
		{
			// TODO: 이게 뭐야!
			GIHit oldHit = Hit;
			Intersection( rtScene, curNode->TriangleIndexArray, curNode->TriangleCount, Ray, Hit );
			if( Hit.isHit() )
			{
				const GITriangle &Triangle = rtScene->GetTriangleList()[Hit.triNum];
				GIVector4 Color = 
					Triangle.vg0.Color * (1.0f - Hit.u - Hit.v) + 
					Triangle.vg1.Color * Hit.u + 
					Triangle.vg2.Color * Hit.v;
				if( Color.w == 0.0f )
				{
					Hit.hit = false;
					continue;
				}

				GIVector3 Normal = 
					Triangle.vg0.Normal * (1.0f - Hit.u - Hit.v) + 
					Triangle.vg1.Normal * Hit.u + 
					Triangle.vg2.Normal * Hit.v;
				if( Normal.DotProduct( Ray.dir ) > 0.0f )
				{
					Hit.backHit = true;
					//Hit = oldHit;
				}
			}

			if( Hit.isHit() )
				break;
			continue;
		}
		else
		{
			int k = curNode->SplitAxis;
			int k1 = modulo[k+1];
			int k2 = modulo[k+2];
			//assert( Ray.dir[k] != 0.0f );

			enum ENodeType { 
				FRONT_NODE = 1, 
				BACK_NODE = 2
			};
			
			// 상대 거리
			float distance_near;
			float distance_far;
			/*float distance_near = BoundingBox.MinPositions.array[k1] - Ray.org[k1];
			float distance_near2 = BoundingBox.MinPositions.array[k2] - Ray.org[k2];
			float distance_far = BoundingBox.MaxPositions.array[k1] - Ray.org[k1];
			float distance_far2 = BoundingBox.MaxPositions.array[k2] - Ray.org[k2];*/

			float distance_split = curNode->SplitPosition - Ray.org[k];
			float boxMin0 = CurStatkNode.BoundingBox.MinPositions.array[k] - Ray.org[k];
			float boxMax0 = CurStatkNode.BoundingBox.MaxPositions.array[k] - Ray.org[k];
			float boxMin1 = CurStatkNode.BoundingBox.MinPositions.array[k1] - Ray.org[k1];
			float boxMax1 = CurStatkNode.BoundingBox.MaxPositions.array[k1] - Ray.org[k1];
			float boxMin2 = CurStatkNode.BoundingBox.MinPositions.array[k2] - Ray.org[k2];
			float boxMax2 = CurStatkNode.BoundingBox.MaxPositions.array[k2] - Ray.org[k2];

			assert( CurStatkNode.BoundingBox.MinPositions.array[k] <= curNode->SplitPosition );
			assert( curNode->SplitPosition <= CurStatkNode.BoundingBox.MaxPositions.array[k] );

			GIBoundingBox LeftBox = CurStatkNode.BoundingBox;
			GIBoundingBox RightBox = CurStatkNode.BoundingBox;

			LeftBox.MaxPositions.array[k] = curNode->SplitPosition;
			RightBox.MinPositions.array[k] = curNode->SplitPosition;

			distance_split = distance_split/Ray.dir[k];
			boxMin0 = boxMin0/Ray.dir[k];
			boxMax0 = boxMax0/Ray.dir[k];
			boxMin1 = boxMin1/Ray.dir[k1];
			boxMax1 = boxMax1/Ray.dir[k1];
			boxMin2 = boxMin2/Ray.dir[k2];
			boxMax2 = boxMax2/Ray.dir[k2];

			if( boxMin0 > boxMax0 )
			{
				float temp = boxMin0;
				boxMin0 = boxMax0;
				boxMax0 = temp;
			}
			if( boxMin1 > boxMax1 )
			{
				float temp = boxMin1;
				boxMin1 = boxMax1;
				boxMax1 = temp;
			}
			if( boxMin2 > boxMax2 )
			{
				float temp = boxMin2;
				boxMin2 = boxMax2;
				boxMax2 = temp;
			}

			distance_near = boxMin1>=boxMin2?boxMin1:boxMin2;
			distance_near = distance_near>=boxMin0?distance_near:boxMin0;
			distance_far = boxMax1<=boxMax2?boxMax1:boxMax2;
			distance_far = distance_far<=boxMax0?distance_far:boxMax0;

			// 박스와 겹치지 않음
			if( distance_far < distance_near )
				continue;

			int Node = 0;
			if( distance_near <= distance_split )
				Node |= FRONT_NODE;
			if( distance_split <= distance_far )
				Node |= BACK_NODE;
			//Node = FRONT_NODE | BACK_NODE;

			if( TraversalFlag[curNode->SplitAxis] == LEFT_FIRST )
			{
				// 왼쪽부터 Traverse : 오른쪽부터 Push
				if( (Node & BACK_NODE) && curNode->RightNode != NULL )
				{
					StackStruct Child = { curNode->RightNode, RightBox };
					Stack.push( Child );
				}
				if( (Node & FRONT_NODE) && curNode->LeftNode != NULL )
				{
					StackStruct Child = { curNode->LeftNode, LeftBox };
					Stack.push( Child );
				}
			}
			else
			{
				if( (Node & BACK_NODE) && curNode->LeftNode != NULL )
				{
					StackStruct Child = { curNode->LeftNode, LeftBox };
					Stack.push( Child );
				}
				if( (Node & FRONT_NODE) && curNode->RightNode != NULL )
				{
					StackStruct Child = { curNode->RightNode, RightBox };
					Stack.push( Child );
				}
			}
			continue;
		}
	}
	return Hit;
	/*if( Hit.isHit() )
	{		
		Shading( rtScene, outColor, Ray, Hit );
	}
	else
	{
		outColor->x = 0.0f;
		outColor->y = 0.0f;
		outColor->z = 0.0f;
		outColor->w = 0.0f;
	}*/


	/*StackStorage.reserve( 0 );
	StackStorage.clear();
	assert( StackStorage.empty() );
	StackStorage.~vector();
	Stack.~stack();*/

	/*_CrtMemCheckpoint( &s2 );
	_CrtMemDifference( &difference, &s1, &s2 );
	_CrtMemDumpStatistics( &difference );*/
}

//GIHit Raytracer::ShootRay( RtScene *rtScene, const GIRay &Ray )
//{
//	RtTriAccel *TriAccelList = rtScene->GetTriAccelList();
//
//	GIHit Hit;
//	Intersection( TriAccelList, rtScene->GetTriangleCount(), Ray, Hit );
//	return Hit;
//}

void Raytracer::TraverseKDTree( RtScene *rtScene, const GIRay &Ray, RtKDTreeNode *pNode, GIHit &Hit )
{	
	assert( pNode != NULL );

	if( pNode->IsLeafNode() )
	{
		Intersection( rtScene, pNode->TriangleIndexArray, pNode->TriangleCount, Ray, Hit );
		return;
	}
	else
	{
		// LeftFirst
		bool LeftFirst = true;

		// 바꿈!
		if( pNode->LeftNode != NULL )
			TraverseKDTree( rtScene, Ray, pNode->LeftNode, Hit );
		if( pNode->RightNode != NULL )
			TraverseKDTree( rtScene, Ray, pNode->RightNode, Hit );
		return;
	}
}

void Raytracer::Shading( RtScene *rtScene, KDTreeStructure *KDTree, const GIRay &Ray, const GIHit &Hit, int MaxDepth, GIVector4 *outColor )
{
	if( Hit.triNum < 0 )
	{
		*outColor = rtScene->GetBackgroundColor();
		return;
	}

	*outColor = GIVector4( 0.0f, 0.0f, 0.0f, 1.0f );

	// TODO: Check!
	/*if( Hit.backHit )
	{
		return;
	}*/

	const GITriangle &Triangle = rtScene->GetTriangleList()[Hit.triNum];

	GIVector3 HitPosition = GIVector3( Ray.org ) + GIVector3( Ray.dir ) * Hit.dist;

	GIVector4 Color = 
		Triangle.vg0.Color * (1.0f - Hit.u - Hit.v) + 
		Triangle.vg1.Color * Hit.u + 
		Triangle.vg2.Color * Hit.v;

	GIVector3 Normal = 
		Triangle.vg0.Normal * (1.0f - Hit.u - Hit.v) + 
		Triangle.vg1.Normal * Hit.u + 
		Triangle.vg2.Normal * Hit.v;

	if( Hit.backHit )
		Normal = -Normal;

	GIVector3 Position = 
		Triangle.vg0.Position * (1.0f - Hit.u - Hit.v) + 
		Triangle.vg1.Position * Hit.u + 
		Triangle.vg2.Position * Hit.v;

	GIVector2 TexCoords = 
		Triangle.vg0.TexCoords * (1.0f - Hit.u - Hit.v) + 
		Triangle.vg1.TexCoords * Hit.u + 
		Triangle.vg2.TexCoords * Hit.v;
 
	GIVector4 AlbedoColor( 0.0f, 0.0f, 0.0f );
	GIVector4 MaterialAmbientColor( 0.0f, 0.0f, 0.0f );
	GIVector4 MaterialDiffuseColor( 0.0f, 0.0f, 0.0f );
	GIVector4 MaterialSpecularColor( 0.0f, 0.0f, 0.0f );
	float MaterialShininess = 0.0f;

	float Alpha = 1.0f;
	
	// TODO: 다시 VERTEX_COLOR_ & Material (NoTexture)
	if( Triangle.HasMaterial()  )
	{
		const int &MaterialIndex = Triangle.MaterialIndex;
		assert( 0 <= MaterialIndex && MaterialIndex < (int)rtScene->GetMaterialCount() );
		// TODO : RELEASE 모드 오류 체크
		GIMaterial *Material = rtScene->GetMaterialArray()[MaterialIndex];

		/*Material->Get
		&& Triangle.TexturingType != GITriangle::ETT_VERTEX_COLOR*/

		AlbedoColor = GIVector4( 1.0f, 1.0f, 1.0f, 1.0f );
		if( Material->GetAlbedoTexture() != NULL )
		{
			assert( Material->GetAlbedoTexture()->GetData() != NULL );

			// TODO : Wrap Mode
			while( TexCoords.u < 0.0f )
				TexCoords.u = TexCoords.u + 1.0f;
			while( TexCoords.u > 1.0f )
				TexCoords.u = TexCoords.u - 1.0f;
			while( TexCoords.v < 0.0f )
				TexCoords.v = TexCoords.v + 1.0f;
			while( TexCoords.v > 1.0f )
				TexCoords.v = TexCoords.v - 1.0f;

			unsigned int x = unsigned int(TexCoords.u * float(Material->GetAlbedoTexture()->GetWidth()) + 0.5f);
			unsigned int y = unsigned int(TexCoords.y * float(Material->GetAlbedoTexture()->GetHeight()) + 0.5f);

			unsigned char *uc = &(Material->GetAlbedoTexture()->GetData()[(x + y * Material->GetAlbedoTexture()->GetWidth())*4]);
			float Pixel[4] = { float(uc[0])/255.0f, float(uc[1])/255.0f, float(uc[2])/255.0f, float(uc[3])/255.0f };
			AlbedoColor = GIVector4( Pixel );
			Alpha = AlbedoColor.w;
		}
		MaterialAmbientColor = AlbedoColor.ComponentWiseProduct( Material->GetAmbientColor() );
		MaterialDiffuseColor = AlbedoColor.ComponentWiseProduct( Material->GetDiffuseColor() );
		MaterialSpecularColor = AlbedoColor.ComponentWiseProduct( Material->GetSpecularColor() );
		MaterialShininess = Material->GetShininess();

		//Material->GetDiffuseTexture()->GetData()
		//MaterialIndex
	}
	else
	{
		MaterialAmbientColor = Color;
		MaterialDiffuseColor = Color;
		/*outColor->x = Color.x;
		outColor->y = Color.y;
		outColor->z = Color.z;*/
		Alpha = 1.0f;
	}

	*outColor = GIVector4( 0.0f, 0.0f, 0.0f );
	for( unsigned int i = 0; i < rtScene->GetLightCount(); i++ )
	{
		//float ShadowFactor = 1.0f - ShadowRays( HitPosition, i );
		float ShadowFactor = 0.0f;
		float VisibilityFactor = 1.0f - ShadowFactor;

		GILight *pLight = rtScene->GetLight( i );
		assert( pLight != NULL );
		// TODO: dynamic casting
		GIPointLight *PointLight = (GIPointLight*)pLight;

		GIVector3 LightVector( PointLight->Position - HitPosition );
		GIVector3 ToLightDirection = LightVector.GetNormalized();	

		GIRay ShadowRay;
		ToLightDirection.CopyToFloatArray( ShadowRay.dir );
		GIVector3 ShadowOrigin( HitPosition + ToLightDirection * 0.001f );
		ShadowOrigin.CopyToFloatArray( ShadowRay.org );
		ShadowRay.NearDistance = 0.001f;
		ShadowRay.FarDistance = LightVector.GetLength();

		// TODO: 완전 임시
		//if( ShadowRay.FarDistance > 50.0f )
		{
			//ShadowRay.FarDistance = 100.0f;
			//continue;
		}

		// Ambient (only when hit?)
		GIVector4 AmbientColor = pLight->AmbientColor.ComponentWiseProduct( MaterialAmbientColor );

		float dist = LightVector.GetLength();
		float Attenuation = 1.0f/(PointLight->ConstantAttenuation + PointLight->LinearAttenuation * dist + PointLight->QuadraticAttenuation * dist * dist);
		//*outColor += AmbientColor;
		*outColor += AmbientColor * Attenuation;	

		if( ToLightDirection.DotProduct( Normal ) <= 0.0f )
			continue;

		GIHit ShadowHit = ShootRay( rtScene, KDTree, ShadowRay );

		if( ShadowHit.isHit() == false )
		{
		// Diffuse
			GIVector4 DiffuseColor = pLight->DiffuseColor.ComponentWiseProduct( MaterialDiffuseColor );

			float DiffuseFactor = Normal.DotProduct( ToLightDirection );
			DiffuseFactor = DiffuseFactor>0.0f?DiffuseFactor:0.0f;

		// Speuclar
			GIVector4 SpecularColor = pLight->SpecularColor.ComponentWiseProduct( MaterialSpecularColor );
			GIVector3 RayVector( Ray.dir );
			GIVector3 HalfVector = (ToLightDirection - RayVector) * 0.5f;
			float SpecularFactor = pow( Normal.DotProduct( HalfVector ), MaterialShininess );
			SpecularFactor = 0.0f;

			*outColor += (DiffuseColor * DiffuseFactor + SpecularColor * SpecularFactor) * VisibilityFactor * Attenuation;
		}
	}
	if( rtScene->GetLightCount() == 0 )
		*outColor = MaterialAmbientColor; //!< Light가 없으면 material의 색깔을 그대로.

	if( MaxDepth > 0 )
	{
		if( Alpha < 0.95 )
		{
			GIVector3 RayOrigin = GIVector3( Ray.org ) + GIVector3( Ray.dir ) * Hit.dist;

			GIRay RefractionRay( Ray );
			RayOrigin.CopyToFloatArray( RefractionRay.org );
			GIHit RefractionHit = ShootRay( rtScene, KDTree, RefractionRay );
			if( RefractionHit.isHit() )
			{
				GIVector4 RefractionColor;
				Shading( rtScene, KDTree, RefractionRay, RefractionHit, MaxDepth-1, &RefractionColor );
				*outColor = *outColor * Alpha + RefractionColor * (1.0f - Alpha);
			}
		}
	}

// TexCoords
	/*outColor->x = TexCoords.s;
	outColor->y = TexCoords.t;
	outColor->z = 0.0f;*/

// Normal
	/*outColor->x = (Normal.x + 1.0f)*0.5f;
	outColor->y = (Normal.y + 1.0f)*0.5f;
	outColor->z = (Normal.z + 1.0f)*0.5f;*/

// Distance
	/*outColor->x = Hit.dist;
	outColor->y = Hit.dist;
	outColor->z = Hit.dist;*/

// Clamp
	outColor->x = outColor->x > 1.0f?1.0f:outColor->x;
	outColor->y = outColor->y > 1.0f?1.0f:outColor->y;
	outColor->z = outColor->z > 1.0f?1.0f:outColor->z;

	outColor->x = outColor->x < 0.0f?0.0f:outColor->x;
	outColor->y = outColor->y < 0.0f?0.0f:outColor->y;
	outColor->z = outColor->z < 0.0f?0.0f:outColor->z;
}

//void Raytracer::Shading( const GIRay &Ray, const &GIHit Hit, GIVector4 *outColor )
//void Test()
//{
//	GITriangle *pTriangleList = Scene->GetTriangleList();
//	//assert( Hit.triNum >= 0 );
//	const GIVertexGroup &vg0 = pTriangleList[Hit.triNum].vg0;
//	const GIVertexGroup &vg1 = pTriangleList[Hit.triNum].vg1;
//	const GIVertexGroup &vg2 = pTriangleList[Hit.triNum].vg2;
//
//	const GIVector3 v0 = vg0.Position;
//	const GIVector3 v1 = vg1.Position;
//	const GIVector3 v2 = vg2.Position;
//
//	float tao = 1.0f - Hit.u - Hit.v;
//	assert( 0.0f <= tao && tao <= 1.0f );
//
//	GIVector3 Position;
//	GIVector2 Texcoord;
//	GIVector3 Normal;
//
//	//Material
//
//	NiTexture.Get
//}

//static const unsigned int modulo[] = {0,1,2,0,1};

void Raytracer::Intersection( RtScene *rtScene, unsigned int *TriangleIndexArray, const int &TriangleCount, const GIRay &Ray, GIHit &Hit )
{
	const RtTriAccel *TriAccelArray = rtScene->GetTriAccelList();
	assert( TriAccelArray != NULL );

	for( int i = 0; i < TriangleCount; i++ )
	{
		unsigned int &TriangleIndex = TriangleIndexArray[i];
		const RtTriAccel &TriAcc = TriAccelArray[TriangleIndex];
		Intersection( TriAcc, Ray, Hit );
	}
}

void Raytracer::Intersection( const RtTriAccel *TriAccelArray, const int &TriangleCount, const GIRay &Ray, GIHit &Hit )
{
	// hit check!
	for( int i = 0; i < TriangleCount; i++ )
		Intersection( TriAccelArray[i], Ray, Hit );
}

void Raytracer::Intersection( const GITriangle &Triangle, const GIRay &Ray, GIHit &Hit )
{
	assert( false );
}

void Raytracer::Intersection( const RtTriAccel &TriAccel, const GIRay &Ray, GIHit &Hit )
{
	RtIntersect( TriAccel, Ray, Hit );
}