/*!
 * \file		PhotonMapping.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:14
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"
#include "../GIEnginePCH.h"

#include "PhotonMapKDTree.h"
#include "PhotonMapping.h"

#include "GIEngine/GIEngine.h"
#include "GIEngine/Raytracer/Raytracer.h"

#include "GIEngine/Raytracer/SceneAccelStructure.h"
#include "GIEngine/Raytracer/KDTreeStructure.h"

#include <Scene.h>
#include <Camera.h>

#include <vector>
#include <stack>

using namespace GIEngine;

static PhotonMapping::PhotonMappingOption g_PhotonMappingOption;

//namespace PhotonMapping {
	void GatherPhotons( std::vector<unsigned int > &outPhoton, PhotonMapping::GIPhotonMapKDTree *pKDTree, const GIVector3 CenterPosition, float Radius, unsigned int Maximum_PhotonCount = 0 );
//};

void PhotonMapping::SetPhotonMappingOption( const PhotonMappingOption &Option )
{
	g_PhotonMappingOption = Option;
}

PhotonMapping::PhotonMappingOption PhotonMapping::GetPhotonMappingOption()
{
	return g_PhotonMappingOption;
}

void PhotonMapping::GeneratePhotons( const GIVector3 &Position, const GIColor3 &Color, unsigned int PhotonCount, GIPhoton *outPhotonArray, SphericalCoordinateSamplingFunction pSphericalCoordinateSamplingFunction )
{
	// TODO: 다른 함수에서 만들었다가 옮기는게 맞을듯.
	// eg. GenerateSamplingRays?()
	for( unsigned int i = 0; i < PhotonCount; i++ )
	{
		outPhotonArray[i].Initialize();
		outPhotonArray[i].Color = Color * g_PhotonMappingOption.PhotonIntensity;
		float theta, phi;
		pSphericalCoordinateSamplingFunction( NormalizedRandom<true,false>(), NormalizedRandom<true,false>(), &theta, &phi );
		outPhotonArray[i].Position = Position;
		outPhotonArray[i].Direction = GIVector3( cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta) );
	}
}

PhotonMapping::GIPhotonMap* PhotonMapping::CreatePhotonMap( GIScene *inScene, GIEngine::SceneAccelStructure *AccelStructure )
{
	if( !g_PhotonMappingOption.IsValid() )
		return NULL;

	if( AccelStructure->GetKDTree() != NULL || !AccelStructure->GetKDTree()->IsBuilt() )
		return NULL;

	/// 완점 임시 코드
	unsigned int PhotonCount = g_PhotonMappingOption.PhotonCountPerLight * inScene->GetLightCount();
	GIPhotonMap *PhotonMap = new GIPhotonMap( inScene, PhotonCount );

	GIPhoton *PhotonArray = new GIPhoton[g_PhotonMappingOption.PhotonCountPerLight];
	GILight **LightArray = inScene->GetLightArray();
	for( unsigned int i = 0; i < inScene->GetLightCount(); i++ )
	{
		// TODO: 현재 Point Light만 지원함. 좀 더 빠른 RTTI 방식으로 대체하거나 다른 방식을 써야함.
		GIPointLight *PointLight = dynamic_cast<GIPointLight*>( LightArray[i] );

		for( unsigned int samplingPass = 0; samplingPass < 2; samplingPass++ )
		{
			// TODO: DiffuseColor를 그대로 쓰는건 말도 안되는 방식
			GeneratePhotons( PointLight->Position, PointLight->DiffuseColor.ToVector3(), g_PhotonMappingOption.PhotonCountPerLight, PhotonArray, &UniformSphericalCoordinateSamplingFunction );

			// TODO: Refactoring 해야함. 죽은 포톤을 계속 loop에서 관리함. 살아 있는 것들만 관리해야함.
			for( unsigned int Pass = 0; Pass <= g_PhotonMappingOption.MaxPhotonPass; Pass++ )
			{
				if( ShootPhotons( Pass, inScene, AccelStructure->GetKDTree(), PhotonArray, g_PhotonMappingOption.PhotonCountPerLight, NULL, NULL ) == false )
					break;

				if( Pass == MAX_PHOTON_SHOOTING_PASS-1 )
				{
					// TOOD: Warning
					break;
				}
			}
			for( unsigned int i = 0; i < g_PhotonMappingOption.PhotonCountPerLight; i++ )
			{
				if( PhotonArray[i].IsValidPhoton() && PhotonArray[i].IsLivingPhoton() == false )
					PhotonMap->InsertPhoton( PhotonArray[i] );
				if( PhotonMap->GetPhotonCount() >= g_PhotonMappingOption.PhotonCountPerLight )
				{
					samplingPass = 100;
					break;
				}
			}
		}
	}

	delete[] PhotonArray;
	PhotonMap->CreatePhotonKDTree();
	return PhotonMap;

	//outPhotonMap->HasKDTree()
}

bool PhotonMapping::ShootPhotons( unsigned int Pass, GIScene *inScene, KDTreeStructure *KDTree, 
				  GIPhoton *inPhotonArray, unsigned int inPhotonCount, 
				  GIPhoton *outPhotonArray/* = NULL*/, unsigned int *outLivingPhotonCount/* = NULL*/ )
{
	assert( g_PhotonMappingOption.IsValid() );
	unsigned int LivingPhotonCount = 0;

	for( unsigned int i = 0; i < inPhotonCount; i++ )
	{
		const GIPhoton &inPhoton = inPhotonArray[i];
		if( inPhoton.IsValidPhoton() && inPhoton.IsLivingPhoton() )
		{
			GIRay ray;
			inPhoton.Position.CopyToFloatArray( ray.org );
			inPhoton.Direction.CopyToFloatArray( ray.dir );
			//ray.NearDistance = inScene->GetSceneEpsilon();
			// 시작 지점에서 epsilon 만큼 이동함.
			// 바꿔야 할 수도 있음.
			ray.NearDistance = 0.0f;
			ray.FarDistance = GI_FLOAT_INFINITY;
			
			GIHit hitResult = Raytracer::ShootRay( inScene, KDTree, ray );

			GIPhoton *outPhoton = NULL;

			// outPhotonArray이 NULL일 경우는 inPhotonArray를 갱신.
			// 그렇지 않을 경우는 변경 사항이 outPhotonArray에 적용됨.
			if( outPhotonArray == NULL )
			{
				outPhoton = &inPhotonArray[i];
			}
			else
			{
				outPhotonArray[i] = inPhoton;
				outPhoton = &outPhotonArray[i];
			}

			if( hitResult.isHit() )
			{
				assert( inScene->GetSceneEpsilon() <= hitResult.dist );
				//const GITriangle &hitTriangle = inScene->GetTriangle( hitResult.triNum );
				// TODO: Albedo는 alpha가 없어야 할 것 같은데...?
				GIVector4 albedo = inScene->GetAlbedo( hitResult.triNum, hitResult.u, hitResult.v );
				GIVector3 normal = inScene->GetNormal( hitResult.triNum, hitResult.u, hitResult.v );

				// 들어가는 방향
				outPhoton->Direction = GIVector3( ray.dir );
				outPhoton->Position = inPhoton.Position + GIVector3( ray.dir ) * hitResult.dist + normal * inScene->GetSceneEpsilon();
				//outPhoton->Color = outPhoton->Color * albedo.ToVector3();

				//outPhoton->Color *= albedo.ToVector3();//GIVector3( 0.0f, 0.0f, 0.0f );

				//
				// Russian Roulette
				// Pass가 0일 경우는 항상 죽지 않음.
				
				bool live = (g_PhotonMappingOption.ExcludeDirectPhoton && Pass == 0) || NormalizedRandom<true,false>() < outPhoton->GetLivingProbability();
				if( live )
				{
					// 원래 BRDF를 고려해야함
					float xi1 = NormalizedRandom<true,false>();
					float xi2 = NormalizedRandom<true,false>();
					float theta, phi;
					UniformHemisphericalCoordinateSamplingFunction( xi1, xi2, &theta, &phi );

					GIVector3 tangent( 0.0f, 0.0f, 0.0f );
					GIVector3 binormal( 0.0f, 0.0f, 0.0f );
					if( inScene->GetTriangle( hitResult.triNum ).HasTangent() )
					{
						tangent = inScene->GetTangent( hitResult.triNum, hitResult.u, hitResult.v );
						binormal = normal.CrossProduct( tangent );
					}
					else
					{
						unsigned int k = 0;
						if( normal.x >= normal.y )
						{
							if( normal.x >= normal.z )
								k = 0;
							else
								k = 2;
						}
						else
						{
							if( normal.y >= normal.z )
								k = 1;
							else
								k = 2;
						}
						unsigned int k2 = (k+1)%3;
						tangent.array[k2] = 1.0;
						binormal = normal.CrossProduct( tangent );
						tangent = binormal.CrossProduct( normal );
						
					}

					// 나가는 방향
					outPhoton->Direction = 
						tangent * sin(theta) * cos( phi ) + 
						binormal * sin(theta) * cos( phi ) + 
						normal * cos(theta);

					// 살아 나갈 경우만 누적
					outPhoton->Color.r *= albedo.ToVector3().r;
					outPhoton->Color.g *= albedo.ToVector3().g;
					outPhoton->Color.b *= albedo.ToVector3().b;

					//if( Pass == 0 )
					//	outPhoton->Color = albedo.ToVector3();
				}
				else
				{
					outPhoton->SetDead();
					outPhoton->mLastPass = Pass;
				}
			}
			else
			{
				// Scene 밖으로 ray가 날아가 없어질 경우(invalid) 최종 방향만 유효하게 저장됨(위치는 유효하지 않음).
				outPhoton->Direction = GIVector3( ray.dir );
				outPhoton->SetInvalid();
				outPhoton->mLastPass = Pass;
				outPhoton->Color = GIVector3( 0.0f, 0.0f, 0.0f );
			}

			// 이번 traverse 후 죽을 수 있음
			if( outPhoton->IsLivingPhoton() )
				LivingPhotonCount++;
		}
	}
	if( outLivingPhotonCount != NULL )
		*outLivingPhotonCount = LivingPhotonCount;
	return LivingPhotonCount>0;
}

GIColor3 PhotonMapping::FinalGatheringCPU( GIScene *inScene, KDTreeStructure *KDTree, GIPhotonMap *inPhotonMap, const GIVector3 &Position, const GIVector3 &RayDirection/*, const GIHit &hitResult*/ )
{
	assert( g_PhotonMappingOption.IsValid() );
	//assert( hitResult.isHit() );
	assert( inPhotonMap != NULL );

	//const GITriangle &hitTriangle = inScene->GetTriangle( hitResult.triNum );

	// 가장 무식한 방법. KDTree도 없고, Sphere로 거리 계산.
	const float SphereRadius = g_PhotonMappingOption.GatheringRadius;
	const float SphereRadiusSquared = SphereRadius * SphereRadius;
	const float SphereArea = 4.0f/3.0f*GI_FLOAT_PI*SphereRadius*SphereRadius;
	
	std::vector<unsigned int> PhotonList;
	PhotonList.reserve( g_PhotonMappingOption.MaximumGatheringPhotonCount );
	GatherPhotons( PhotonList, inPhotonMap->GetPhotonMapKDTree(), Position, g_PhotonMappingOption.GatheringRadius, g_PhotonMappingOption.MaximumGatheringPhotonCount );

	GIColor3 outColor( 0.0f, 0.0f, 0.0f );
	for( unsigned int i = 0; i < PhotonList.size(); i++ )
	{
		const GIPhoton &Photon = inPhotonMap->GetPhotonArray()[PhotonList[i]];
		assert( Photon.IsValidPhoton() && Photon.IsLivingPhoton() == false );
		GIVector3 distanceVector = Photon.Position - Position;
		float distanceSqaured = (distanceVector).GetLengthSquared();

		if( distanceSqaured < SphereRadiusSquared )
		{
			float distance = sqrt( distanceSqaured );
			outColor += Photon.Color * (1.0f-distance/SphereRadius);
			//outColor += Photon.Color;
		}
	}

	//GIColor3 outColor( 0.0f, 0.0f, 0.0f );
	//for( unsigned int i = 0; i < inPhotonMap->GetPhotonCount(); i++ )
	//{
	//	const GIPhoton &Photon = inPhotonMap->GetPhotonArray()[i];
	//	assert( Photon.IsValidPhoton() && Photon.IsLivingPhoton() == false );
	//	GIVector3 distanceVector = Photon.Position - Position;
	//	if( (distanceVector).GetLengthSquared() < SphereRadiusSquared )
	//	{
	//		//float sigma_squred = 
	//		outColor += Photon.Color;
	//	}
	//}

	outColor /= SphereArea;
	return outColor;
}

PhotonMapping::GIPhotonMap *gPhotonMap = NULL;

void PhotonMappingShading( GIScene *rtScene, KDTreeStructure *KDTree, const GIRay &Ray, const GIHit &Hit, int MaxDepth, GIVector4 *outColor )
{
	GIColor4 DirectIlluminationColor;
	Raytracer::Shading( rtScene, KDTree, Ray, Hit, MaxDepth, &DirectIlluminationColor );
	GIColor3 IndirectIlluminationColor( 0.0f, 0.0f, 0.0f );
	if( Hit.isHit() )
		IndirectIlluminationColor = PhotonMapping::FinalGatheringCPU( (GIScene*)rtScene, KDTree, gPhotonMap, GIVector3( Ray.org ) + GIVector3( Ray.dir ) * Hit.dist, GIVector3( Ray.dir ) );;
	
	// TODO: Clamp
	if( IndirectIlluminationColor.r > 1.0f )
		IndirectIlluminationColor.r = 1.0;
	if( IndirectIlluminationColor.g > 1.0f )
		IndirectIlluminationColor.g = 1.0;
	if( IndirectIlluminationColor.b > 1.0f )
		IndirectIlluminationColor.b = 1.0;
	//IndirectIlluminationColor.r = pow( IndirectIlluminationColor.r, 1.0f/2.2f );
	//IndirectIlluminationColor.g = pow( IndirectIlluminationColor.g, 1.0f/2.2f );
	//IndirectIlluminationColor.b = pow( IndirectIlluminationColor.b, 1.0f/2.2f );
	*outColor = DirectIlluminationColor + GIVector4( IndirectIlluminationColor.r, IndirectIlluminationColor.g, IndirectIlluminationColor.b, 0.0f );
}

void PhotonMapping::Render( unsigned int ThreadCount, GIScene *pScene, SceneAccelStructure *AccelStructure, GIPhotonMap *pPhotonMap, GICamera *pCamera, GIVector3 *outPixelData )
{
	gPhotonMap = pPhotonMap;

	unsigned int Width = pCamera->Width;
	unsigned int Height = pCamera->Height;
	unsigned int RayCount = Width * Height;

	GIRay *RayArray = new GIRay[RayCount];
	GenerateRays( pCamera, RayArray );

	// TODO: Error Check
	Raytracer::UserDefinedShadingCPU( ThreadCount, pScene, AccelStructure->GetKDTree(), RayArray, RayCount, outPixelData, &PhotonMappingShading );
	delete[] RayArray;
}

//////////////////////////////////////////////////////////////////////////
void GatherPhotons( std::vector<unsigned int > &outPhoton, PhotonMapping::GIPhotonMapKDTree *pKDTree, const GIVector3 GatheringPosition, float GatheringRadius, unsigned int Maximum_PhotonCount )
{
	assert( pKDTree->IsBuilt() );

	//_CrtMemState s1, s2, difference;
	//_CrtMemCheckpoint( &s1 );

	// TODO: 속도 문제가 좀 있을 수 있음.
	struct StackStruct {
		PhotonMapping::GIPhotonMapKDTreeNode *pNode;
		GIBoundingBox BoundingBox;
	};

	StackStruct RootStatkNode = { pKDTree->GetRootNode(), pKDTree->GetBoundingBox() };

	std::vector<StackStruct> StackStorage;
	StackStorage.reserve( pKDTree->GetNodeSize() * 2 );
	std::stack<StackStruct, std::vector<StackStruct> > Stack( StackStorage );
	Stack.push( RootStatkNode );

	// assume Ray hit pNode

	static int modulo[5] = { 0, 1, 2, 0, 1 };

	while( Stack.empty() == false )
	{
		StackStruct CurStatkNode = Stack.top();
		PhotonMapping::GIPhotonMapKDTreeNode *curNode = CurStatkNode.pNode;
		const GIBoundingBox &BB = CurStatkNode.BoundingBox;
		const GIVector3 &Bmin = BB.MinPositions;
		const GIVector3 &Bmax = BB.MaxPositions;
		Stack.pop();

		assert( curNode != NULL );

		float dmin = 0;
		float r2 = GatheringRadius * GatheringRadius;
		for( unsigned int i = 0; i < 3; i++ ) {
			if( GatheringPosition.array[i] < Bmin.array[i] )
			{
				//dmin += sqrt( GatheringPosition.array[i] - Bmin.array[i] );
				float a = Bmin.array[i] - GatheringPosition.array[i];
				dmin += a * a;
				
			}
			else if( GatheringPosition.array[i] > Bmax.array[i] )
			{
				//dmin += sqrt( GatheringPosition.array[i] - Bmax.array[i] );
				float a = GatheringPosition.array[i] - Bmax.array[i];
				dmin += a * a;
			}
		}

		/*if( dmin <= r2 )
			return TRUE;*/
		if( dmin > r2 )
			continue;

		// GatheringPosition은 이 노드안에 들어왔음.
		if( curNode->IsLeafNode() )
		{
			// 거리 비교를 안하고 일단 다 넣음.
			for( unsigned int i = 0; i < curNode->PhotonCount && outPhoton.size() < Maximum_PhotonCount; i++ )
				outPhoton.push_back( curNode->PhotonIndexArray[i] );

			if( Maximum_PhotonCount <= outPhoton.size() )
				return;

			continue;
		}
		else
		{
			int k = curNode->SplitAxis;
			int k1 = modulo[k+1];
			int k2 = modulo[k+2];

			enum ENodeType { 
				FRONT_NODE = 1, 
				BACK_NODE = 2
			};

			int Node = 0;
			if( GatheringPosition.array[k] - GatheringRadius <= curNode->SplitPosition )
				Node |= FRONT_NODE;
			if( curNode->SplitPosition <= GatheringPosition.array[k] + GatheringRadius )
				Node |= BACK_NODE;
			assert( Node > 0 );

			GIBoundingBox LeftBox = CurStatkNode.BoundingBox;
			GIBoundingBox RightBox = CurStatkNode.BoundingBox;

			LeftBox.MaxPositions.array[k] = curNode->SplitPosition;
			RightBox.MinPositions.array[k] = curNode->SplitPosition;

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
			continue;
		}
	}
	return;
}
