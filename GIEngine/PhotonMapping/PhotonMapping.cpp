#include "GIEnginePCH.h"
#include "../GIEnginePCH.h"

#include "PhotonMapping.h"

#include "GIEngine/GIEngine.h"
#include "GIEngine/GIScene.h"
#include "GIEngine/Raytracer/Raytracer.h"

using namespace GIEngine;

void PhotonMapping::GeneratePhotons( const GIVector3 &Position, const GIColor3 &Color, unsigned int PhotonCount, GIPhoton *outPhotonArray, SphericalCoordinateSamplingFunction pSphericalCoordinateSamplingFunction )
{
	// TODO: 다른 함수에서 만들었다가 옮기는게 맞을듯.
	// eg. GenerateSamplingRays?()
	for( unsigned int i = 0; i < PhotonCount; i++ )
	{
		outPhotonArray[i].Initialize();
		outPhotonArray[i].Color = Color;
		float theta, phi;
		pSphericalCoordinateSamplingFunction( NormalizedRandom<true,false>(), NormalizedRandom<true,false>(), &theta, &phi );
		outPhotonArray[i].Position = Position;
		outPhotonArray[i].Direction = GIVector3( cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta) );
	}
}

PhotonMapping::GIPhotonMap* PhotonMapping::CreatePhotonMap( GIScene *inScene, unsigned int MaxPhotonPass )
{
	if( MaxPhotonPass <= 1 )
	{
		// 최소 2번의 pass는 거쳐야함.
		// TODO: Warning
		return NULL;
	}

	/// 완점 임시 코드
	unsigned int PhotonCount = 10000000;
	GIPhotonMap *PhotonMap = new GIPhotonMap( inScene, PhotonCount * inScene->GetLightCount() );

	GIPhoton *PhotonArray = new GIPhoton[PhotonCount];
	GILight **LightArray = inScene->GetLightArray();
	for( unsigned int i = 0; i < inScene->GetLightCount(); i++ )
	{
		// TODO: 현재 Point Light만 지원함. 좀 더 빠른 RTTI 방식으로 대체하거나 다른 방식을 써야함.
		GIPointLight *PointLight = dynamic_cast<GIPointLight*>( LightArray[i] );

		for( unsigned int samplingPass = 0; samplingPass < 2; samplingPass++ )
		{
			// TODO: DiffuseColor를 그대로 쓰는건 말도 안되는 방식
			GeneratePhotons( PointLight->Position, PointLight->DiffuseColor.ToVector3(), PhotonCount, PhotonArray, &UniformSphericalCoordinateSamplingFunction );

			// TODO: Refactoring 해야함. 죽은 포톤을 계속 loop에서 관리함. 살아 있는 것들만 관리해야함.
			for( unsigned int Pass = 0; Pass <= MaxPhotonPass; Pass++ )
			{
				if( ShootPhotons( Pass, inScene, PhotonArray, PhotonCount, NULL, NULL ) == false )
					break;

				if( Pass == MAX_PHOTON_SHOOTING_PASS-1 )
				{
					// TOOD: Warning
					break;
				}
			}
			for( unsigned int i = 0; i < PhotonCount; i++ )
			{
				if( PhotonArray[i].IsValidPhoton() && PhotonArray[i].IsLivingPhoton() == false )
					PhotonMap->InsertPhoton( PhotonArray[i] );
				if( PhotonMap->GetPhotonCount() >= PhotonCount )
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

bool PhotonMapping::ShootPhotons( unsigned int Pass, GIScene *inScene, 
				  GIPhoton *inPhotonArray, unsigned int inPhotonCount, 
				  GIPhoton *outPhotonArray/* = NULL*/, unsigned int *outLivingPhotonCount/* = NULL*/ )
{
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
			ray.FarDistance = GI_INFINITY;
			
			GIHit hitResult = Raytracer::ShootRay( inScene, ray );

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

				if( Pass == 0 )
					outPhoton->Color = albedo.ToVector3();//GIVector3( 0.0f, 0.0f, 0.0f );
				//if( Pass == 1 )
				//	outPhoton->Color = albedo.ToVector3();

				//
				// Russian Roulette
				// Pass가 0일 경우는 항상 죽지 않음.
				bool live = Pass == 0 || NormalizedRandom<true,false>() < outPhoton->GetLivingProbability();
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

GIColor3 PhotonMapping::FinalGathering( GIScene *inScene, GIPhotonMap *inPhotonMap, const GIVector3 &Position, const GIVector3 &RayDirection/*, const GIHit &hitResult*/ )
{
	//assert( hitResult.isHit() );

	//const GITriangle &hitTriangle = inScene->GetTriangle( hitResult.triNum );

	// 가장 무식한 방법. KDTree도 없고, Sphere로 거리 계산.
	const float SphereRadius = 10.0f;
	const float SphereRadiusSquared = SphereRadius * SphereRadius;
	const float SphereArea = 4.0f/3.0f*FLOAT_PI*SphereRadius*SphereRadius*SphereRadius;
	GIColor3 outColor( 0.0f, 0.0f, 0.0f );
	for( unsigned int i = 0; i < inPhotonMap->GetPhotonCount(); i++ )
	{
		const GIPhoton &Photon = inPhotonMap->GetPhotonArray()[i];
		assert( Photon.IsValidPhoton() && Photon.IsLivingPhoton() == false );
		if( (Position - Photon.Position).GetLengthSquared() < SphereRadius )
		{
			outColor += Photon.Color;
		}
	}
	outColor /= SphereArea;
	return outColor;
}

PhotonMapping::GIPhotonMap *gPhotonMap = NULL;

void PhotonMappingShading( RtScene *rtScene, const GIRay &Ray, const GIHit &Hit, int MaxDepth, GIVector4 *outColor )
{
	GIColor4 DirectIlluminationColor;
	Raytracer::Shading( rtScene, Ray, Hit, MaxDepth, &DirectIlluminationColor );
	GIColor3 IndirectIlluminationColor( 0.0f, 0.0f, 0.0f );
	if( Hit.isHit() )
		IndirectIlluminationColor = PhotonMapping::FinalGathering( (GIScene*)rtScene, gPhotonMap, GIVector3( Ray.org ) + GIVector3( Ray.dir ) * Hit.dist, GIVector3( Ray.dir ) );;
	
	*outColor = DirectIlluminationColor + GIVector4( IndirectIlluminationColor.r, IndirectIlluminationColor.g, IndirectIlluminationColor.b, 0.0f );
}

void PhotonMapping::Render( unsigned int ThreadCount, GIScene *pScene, GIPhotonMap *pPhotonMap, GICamera *pCamera, GIVector3 *outPixelData )
{
	gPhotonMap = pPhotonMap;

	unsigned int Width = pCamera->Width;
	unsigned int Height = pCamera->Height;
	unsigned int RayCount = Width * Height;

	GIRay *RayArray = new GIRay[RayCount];
	GenerateRays( pCamera, RayArray );

	UserDefinedShading( ThreadCount, pScene, RayCount, RayArray, outPixelData, &PhotonMappingShading );
	delete[] RayArray;
}