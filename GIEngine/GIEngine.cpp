/*!
 * \file		GIEngine.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:13
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include "GIScene.h"

#include "SphericalHarmonics/SphericalHarmonics.h"
#include "AmbientCube/AmbientCube.h"

#include "AmbientCube/AmbientCubeIrradianceVolume.h"

#include "GIEngine.h"

//#include "Raytracer/Raytracer.h"

#include "Raytracer/KDTreeStructure.h"

#include "Raytracer/RaytracerProcedure.h"
#include "IrradianceCalcThread.h"

RtScene* RaytracerProcedure::mRtScene = NULL;
KDTreeStructure* RaytracerProcedure::mKDTree = NULL;

static float gRayShootEpsilon = 0.00001f;

bool GIEngine::Initialize()
{
	return true;
}

void GIEngine::Finalize()
{
}

//GIScene* GIEngine::GISceneManager::CreateScene()
//{
//	return new GIScene;
//}

void GIEngine::GenerateRays( GICamera *pCamera, GIRay *outRayArray )
{
	bool WidthIsOdd = pCamera->Width % 2 == 1;
	bool HeightIsOdd = pCamera->Height % 2 == 1;

	unsigned int HalfWidth = pCamera->Width/2;
	unsigned int HalfHeight = pCamera->Height/2;

	// TODO: odd 처리

	// TODO: 각종 ASSERT
	GIVector3 Origin = pCamera->Origin;

	GIVector3 Direction = pCamera->Direction.GetNormalized();
	GIVector3 UpDirection = pCamera->UpDirection.GetNormalized();
	GIVector3 DownDirection = -UpDirection;
	GIVector3 RightDirection = Direction.CrossProduct( UpDirection ).GetNormalized();

	float aspect = float(pCamera->Width)/float(pCamera->Height);

	// World Space 의 길이
	float fHalfHeight = tan( pCamera->Fovy/2.0f * FLOAT_PI /180.0f );
	float fHalfWidth = fHalfHeight * aspect;

	GIVector3 UpperLeft = Direction + RightDirection * -float(fHalfWidth) + UpDirection * float(fHalfHeight);
	GIVector3 RightDirectionDelta = RightDirection * fHalfWidth / float(HalfWidth);
	GIVector3 DownDirectionDelta = DownDirection * fHalfHeight / float(HalfHeight);

	GIVector3 TestTarget = Origin + UpperLeft + RightDirectionDelta * float(HalfWidth) + DownDirectionDelta * float(HalfHeight);
	GIVector3 TestDirection = (TestTarget - Origin).GetNormalized();

	unsigned int index = 0;
	for( unsigned int y = 0; y < pCamera->Height; y++ )
		for( unsigned int x = 0; x < pCamera->Width; x++ )
		{
			GIVector3 RayTarget = Origin + UpperLeft + RightDirectionDelta * float(x) + DownDirectionDelta * float(y);
			GIVector3 RayDirection = (RayTarget - Origin).GetNormalized();

			GIRay ray;
			RayDirection.CopyToFloatArray( ray.dir );
			ray.NearDistance = 0.0f;
			ray.FarDistance = 1e16f;
			Origin.CopyToFloatArray( ray.org );

			outRayArray[index++] = ray;
		}
}

void GIEngine::GenerateSphericalSampleArray( const GIVector3 &Position, SphericalSampleArray *inoutSphericalSampleArray )
{
	for( unsigned int i = 0; i < inoutSphericalSampleArray->GetRayCount(); i++ )
	{
		float xi1 = NormalizedRandom();
		float xi2 = NormalizedRandom<true, false>();

		float theta = 2.0f * acos( sqrt( 1.0f - xi1 ) );
		float phi = 2.0f * FLOAT_PI * xi2;

		if( inoutSphericalSampleArray->GetRayArray() )
		{
			GIVector3 RayDirection( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) );
			float length = RayDirection.GetLength();

			Position.CopyToFloatArray( inoutSphericalSampleArray->GetRayArray()[i].org );
			RayDirection.CopyToFloatArray( inoutSphericalSampleArray->GetRayArray()[i].dir );

			inoutSphericalSampleArray->GetRayArray()[i].NearDistance = 0.0001f;
			inoutSphericalSampleArray->GetRayArray()[i].FarDistance = GI_INFINITY;
		}

		if( inoutSphericalSampleArray->GetThetaArray() != NULL )
			inoutSphericalSampleArray->GetThetaArray()[i] = theta;
		if( inoutSphericalSampleArray->GetPhiArray() != NULL )
			inoutSphericalSampleArray->GetPhiArray()[i] = phi;
	}
}

void GIEngine::GenerateSphericalSampleArray( const GIVector3 &Position, const GIVector3 &Normal, SphericalSampleArray *inoutSphericalSampleArray )
{
	for( unsigned int i = 0; i < inoutSphericalSampleArray->GetRayCount(); i++ )
	{
		float xi1 = NormalizedRandom();
		float xi2 = NormalizedRandom<true, false>();

		float theta = 2.0f * acos( sqrt( 1.0f - xi1 ) );
		float phi = 2.0f * FLOAT_PI * xi2;

		GIVector3 RayDirection( sin( theta ) * cos( phi ), sin( theta ) * sin( phi ), cos( theta ) );
		if( inoutSphericalSampleArray->GetRayArray() )
		{
			float length = RayDirection.GetLength();

			Position.CopyToFloatArray( inoutSphericalSampleArray->GetRayArray()[i].org );
			RayDirection.CopyToFloatArray( inoutSphericalSampleArray->GetRayArray()[i].dir );

			inoutSphericalSampleArray->GetRayArray()[i].NearDistance = 0.0001f;
			inoutSphericalSampleArray->GetRayArray()[i].FarDistance = GI_INFINITY;
		}
		if( inoutSphericalSampleArray->GetThetaArray() != NULL )
			inoutSphericalSampleArray->GetThetaArray()[i] = theta;
		if( inoutSphericalSampleArray->GetPhiArray() != NULL )
			inoutSphericalSampleArray->GetPhiArray()[i] = phi;
		if( inoutSphericalSampleArray->GetWeightArray() != NULL )
			inoutSphericalSampleArray->GetWeightArray()[i] = RayDirection.DotProduct( Normal.GetNormalized() );;
	}
}

void GIEngine::GenerateHemisphericalSampleArray( const GIVector3 &Position, const GIVector3 &Normal, SphericalSampleArray *inoutSphericalSampleArray )
{
	int k = fabs(Normal.x) >= fabs(Normal.y) ? 0:1;
	if( k == 0 )
		k = fabs(Normal.x) >= fabs(Normal.z) ? 0:2;
	else
		k = fabs(Normal.y) >= fabs(Normal.z) ? 1:2;

	int k1 = (k+1)%3;
	//int k1 = k1<<1&3;

	GIVector3 Tangent( 0.0f, 0.0f, 0.0f );
	Tangent.array[k1] = 1.0f;

	GIVector3 Binormal = Normal.CrossProduct( Tangent ).GetNormalized();
	Tangent = Binormal.CrossProduct( Normal ).GetNormalized();

	bool bRay = inoutSphericalSampleArray->GetRayArray() != NULL;
	bool bSpherical = inoutSphericalSampleArray->GetThetaArray() != NULL && inoutSphericalSampleArray->GetPhiArray() != NULL;
	bool bWeighted = inoutSphericalSampleArray->GetWeightArray() != NULL;

	for( unsigned int i = 0; i < inoutSphericalSampleArray->GetRayCount(); i++ )
	{
		float xi1 = NormalizedRandom();
		float xi2 = NormalizedRandom<true, false>();
		//float xi1_squared = xi1*xi1;

		float theta = acos( xi1 );
		//float theta=2.0f*acos(sqrt(1.0-x));
		float phi = 2.0f * FLOAT_PI * xi2;

		float x = sin(theta) * cos( phi );
		float y = sin(theta) * sin( phi );
		float z = cos(theta);

		//float x = sqrt( 1.0f - xi1_squared ) * cos( phi );
		//float y = sqrt( 1.0f - xi1_squared ) * sin( phi );
		//float z = xi1; //!< normal axis

		float length = x*x+y*y+z*z;
		x /= sqrt(length);
		y /= sqrt(length);
		z /= sqrt(length);

		GIVector3 RayDirection = Tangent * x + Binormal * y + Normal * z;
		if( bRay )
		{
			GIRay &outRay = inoutSphericalSampleArray->GetRayArray()[i];
			
			Position.CopyToFloatArray( outRay.org );
			RayDirection.CopyToFloatArray( outRay.dir );
			outRay.NearDistance = 0.0001f;
			outRay.FarDistance = GI_INFINITY;
		}
		if( bSpherical )
		{
			inoutSphericalSampleArray->GetThetaArray()[i] = theta;
			inoutSphericalSampleArray->GetPhiArray()[i] = phi;
		}
		if( bWeighted )
		{
			inoutSphericalSampleArray->GetWeightArray()[i] = Normal.DotProduct( RayDirection );
			/*if( inoutSphericalSampleArray->GetWeightArray()[i] < 0.0f )
				int a = 0;
			assert( 0.0f <= inoutSphericalSampleArray->GetWeightArray()[i] );*/
		}
	}
}

bool GIEngine::BuildAccStructure( GIScene *Scene, const char *strStructureFilename )
{
	assert( Scene );

	if( Scene->GetAccStructure() != NULL )
	{
		delete Scene->GetAccStructure();
		Scene->SetAccStructure( NULL );
	}
	return Raytracer::BuildStructure( (RtScene*)Scene, strStructureFilename, true );
}

void GIEngine::Render( unsigned int ThreadCount, GIScene *pScene, GICamera *pCamera, GIVector3 *outPixelData )
{
	unsigned int Width = pCamera->Width;
	unsigned int Height = pCamera->Height;
	unsigned int RayCount = Width * Height;

	GIRay *RayArray = new GIRay[RayCount];
	GenerateRays( pCamera, RayArray );

	SampleColor( ThreadCount, pScene, RayCount, RayArray, outPixelData );
	delete[] RayArray;
}

void GIEngine::SampleColor( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray )
{
	
	assert( pScene != NULL && RayArray != NULL && ThreadCount > 0 && outColorArray != NULL && RayCount > 0 );

	KDTreeStructure *pKDTree = (KDTreeStructure*)pScene->GetAccStructure();

	if( pKDTree == NULL || pKDTree->IsBuilt() == false )
	{
		return;
	}

	// non-thread
	if( ThreadCount <= 1 )
	{
		GIVector4 RenderedColor;
		for( unsigned int i = 0; i < RayCount; i++ )
		{
			// KD-Tree
			GIHit Hit = Raytracer::ShootRay( pScene, RayArray[i] );
			Raytracer::Shading( pScene, RayArray[i], Hit, 2, &RenderedColor );
			outColorArray[i] = RenderedColor.ToVector3();
		}
		return;
	}

	// thread
	RaytracerProcedure *pRaytracerProcedureArray;
	pRaytracerProcedureArray = new RaytracerProcedure[ThreadCount];

	unsigned int RayCountPerThread = RayCount/ThreadCount;
	int RestRayCount = (int(RayCount) - int(RayCountPerThread * ThreadCount));
	assert( 0 <= RestRayCount );

	RaytracerProcedure::mRtScene = pScene;
	RaytracerProcedure::mKDTree = pKDTree;

	// TODO: MAX_THREAD?
	LPDWORD pThreadIdArray = new DWORD[ThreadCount];
	LPHANDLE pThreadArray = new HANDLE[ThreadCount];

	unsigned int RayIndex = 0;
	for( unsigned int i = 0; i < ThreadCount; i++ )
	{
		RaytracerProcedure *pRaytracerProcedure = &((RaytracerProcedure*)pRaytracerProcedureArray)[i];

		pRaytracerProcedure->mRayArray = &RayArray[RayIndex];
		pRaytracerProcedure->mRayCount = RayCountPerThread;
		pRaytracerProcedure->mOutData = &outColorArray[RayIndex];

		pThreadArray[i] = CreateThread( 
			NULL,							// default security attributes
			0,								// use default stack size  
			RaytracerProcedure::ShadingProcedure,       // thread function name
			//NULL, 
			(LPVOID)pRaytracerProcedure,  // argument to thread function 
			CREATE_SUSPENDED,
			//NULL );
			&pThreadIdArray[i] );			// returns the thread identifier 

		RayIndex+= RayCountPerThread;
	}

	RaytracerProcedure *pLastRaytracerProcedure = &((RaytracerProcedure*)&pRaytracerProcedureArray)[ThreadCount-1];
	//pLastRaytracerProcedure->mRayCount += RestRayCount;

	for( unsigned int i = 0; i < ThreadCount; i++ )
		ResumeThread( pThreadArray[i] );

	WaitForMultipleObjects( ThreadCount, pThreadArray, TRUE, INFINITE );

	for( unsigned int i = 0; i < ThreadCount; i++ )
		CloseHandle( pThreadArray[i] );

	delete[] pThreadArray;
	delete[] pThreadIdArray;
	delete[] pRaytracerProcedureArray;
}

void GIEngine::SampleDistance( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, float *outDistanceArray )
{
	
	assert( pScene != NULL && RayArray != NULL && ThreadCount > 0 && outDistanceArray != NULL && RayCount > 0 );

	KDTreeStructure *pKDTree = (KDTreeStructure*)pScene->GetAccStructure();

	if( pKDTree == NULL || pKDTree->IsBuilt() == false )
	{
		return;
	}

	// non-thread
	if( ThreadCount <= 1 )
	{
		GIVector4 RenderedColor;
		for( unsigned int i = 0; i < RayCount; i++ )
		{
			// KD-Tree
			GIHit Hit = Raytracer::ShootRay( pScene, RayArray[i] );

			float distance = Hit.dist;
			bool hit = Hit.isHit();
			bool frontHit = !Hit.backHit;
			if( hit )
			{
				if( frontHit )
					outDistanceArray[i] = distance;
				else
					outDistanceArray[i] = 0.0f;
			}
			else
				outDistanceArray[i] = -1.0f;
		}
		return;
	}

	// thread
	RaytracerProcedure *pRaytracerProcedureArray;
	pRaytracerProcedureArray = new RaytracerProcedure[ThreadCount];

	unsigned int RayCountPerThread = RayCount/ThreadCount;
	int RestRayCount = (int(RayCount) - int(RayCountPerThread * ThreadCount));
	assert( 0 <= RestRayCount );

	RaytracerProcedure::mRtScene = pScene;
	RaytracerProcedure::mKDTree = pKDTree;

	//GIVector3 *TempVector = new GIVector3[RayCount];
	GIHit *HitArray = new GIHit[RayCount];

	// TODO: MAX_THREAD?
	LPDWORD pThreadIdArray = new DWORD[ThreadCount];
	LPHANDLE pThreadArray = new HANDLE[ThreadCount];

	unsigned int RayIndex = 0;
	for( unsigned int i = 0; i < ThreadCount; i++ )
	{
		RaytracerProcedure *pRaytracerProcedure = &((RaytracerProcedure*)pRaytracerProcedureArray)[i];

		pRaytracerProcedure->mRayArray = &RayArray[RayIndex];
		pRaytracerProcedure->mRayCount = RayCountPerThread;
		pRaytracerProcedure->mOutData = &HitArray[RayIndex];

		pThreadArray[i] = CreateThread( 
			NULL,							// default security attributes
			0,								// use default stack size  
			RaytracerProcedure::HitProcedure,       // thread function name
			//NULL, 
			(LPVOID)pRaytracerProcedure,  // argument to thread function 
			CREATE_SUSPENDED,
			//NULL );
			&pThreadIdArray[i] );			// returns the thread identifier 

		RayIndex+= RayCountPerThread;
	}

	RaytracerProcedure *pLastRaytracerProcedure = &((RaytracerProcedure*)&pRaytracerProcedureArray)[ThreadCount-1];
	//pLastRaytracerProcedure->mRayCount += RestRayCount;

	for( unsigned int i = 0; i < ThreadCount; i++ )
		ResumeThread( pThreadArray[i] );

	WaitForMultipleObjects( ThreadCount, pThreadArray, TRUE, INFINITE );

	for( unsigned int i = 0; i < ThreadCount; i++ )
		CloseHandle( pThreadArray[i] );

	// 히트일 경우 거리, 그렇지 않을 경우 음수
	// backhit일 경우 0에서 hit했다고 가정
	for( unsigned int i = 0; i < RayCount; i++ )
	{
		if( HitArray[i].isHit() )
		{
			if( !HitArray[i].backHit )
				outDistanceArray[i] = HitArray[i].dist;
			else
				outDistanceArray[i] = 0.0f;
		}
		else
			outDistanceArray[i] = -1.0f;
	}

	delete[] HitArray;
	delete[] pThreadArray;
	delete[] pThreadIdArray;
	delete[] pRaytracerProcedureArray;
}

void GIEngine::SampleHit( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, GIHit *outHitArray )
{
	
	assert( pScene != NULL && RayArray != NULL && ThreadCount > 0 && outHitArray != NULL && RayCount > 0 );

	KDTreeStructure *pKDTree = (KDTreeStructure*)pScene->GetAccStructure();

	if( pKDTree == NULL || pKDTree->IsBuilt() == false )
	{
		return;
	}

	// non-thread
	if( ThreadCount <= 1 )
	{
		GIVector4 RenderedColor;
		for( unsigned int i = 0; i < RayCount; i++ )
		{
			// KD-Tree
			outHitArray[i] = Raytracer::ShootRay( pScene, RayArray[i] );
		}
		return;
	}

	// thread
	RaytracerProcedure *pRaytracerProcedureArray;
	pRaytracerProcedureArray = new RaytracerProcedure[ThreadCount];

	unsigned int RayCountPerThread = RayCount/ThreadCount;
	int RestRayCount = (int(RayCount) - int(RayCountPerThread * ThreadCount));
	assert( 0 <= RestRayCount );

	RaytracerProcedure::mRtScene = pScene;
	RaytracerProcedure::mKDTree = pKDTree;

	// TODO: MAX_THREAD?
	LPDWORD pThreadIdArray = new DWORD[ThreadCount];
	LPHANDLE pThreadArray = new HANDLE[ThreadCount];

	unsigned int RayIndex = 0;
	for( unsigned int i = 0; i < ThreadCount; i++ )
	{
		RaytracerProcedure *pRaytracerProcedure = &((RaytracerProcedure*)pRaytracerProcedureArray)[i];

		pRaytracerProcedure->mRayArray = &RayArray[RayIndex];
		pRaytracerProcedure->mRayCount = RayCountPerThread;
		pRaytracerProcedure->mOutData = &outHitArray[RayIndex];

		pThreadArray[i] = CreateThread( 
			NULL,							// default security attributes
			0,								// use default stack size  
			RaytracerProcedure::HitProcedure,       // thread function name
			//NULL, 
			(LPVOID)pRaytracerProcedure,  // argument to thread function 
			CREATE_SUSPENDED,
			//NULL );
			&pThreadIdArray[i] );			// returns the thread identifier 

		RayIndex+= RayCountPerThread;
	}

	RaytracerProcedure *pLastRaytracerProcedure = &((RaytracerProcedure*)&pRaytracerProcedureArray)[ThreadCount-1];
	//pLastRaytracerProcedure->mRayCount += RestRayCount;

	for( unsigned int i = 0; i < ThreadCount; i++ )
		ResumeThread( pThreadArray[i] );

	WaitForMultipleObjects( ThreadCount, pThreadArray, TRUE, INFINITE );

	for( unsigned int i = 0; i < ThreadCount; i++ )
		CloseHandle( pThreadArray[i] );

	delete[] pThreadArray;
	delete[] pThreadIdArray;
	delete[] pRaytracerProcedureArray;
}

void GIEngine::UserDefinedShading( unsigned int ThreadCount, GIScene *pScene, unsigned int RayCount, GIRay *RayArray, GIVector3 *outColorArray, ShadingFunction pfnShadingFunction ) 
{
	assert( pScene != NULL && RayArray != NULL && ThreadCount > 0 && outColorArray != NULL && RayCount > 0 );

	KDTreeStructure *pKDTree = (KDTreeStructure*)pScene->GetAccStructure();

	if( pKDTree == NULL || pKDTree->IsBuilt() == false )
	{
		return;
	}

	// non-thread
	if( ThreadCount <= 1 )
	{
		GIVector4 RenderedColor;
		for( unsigned int i = 0; i < RayCount; i++ )
		{
			// KD-Tree
			GIHit Hit = Raytracer::ShootRay( pScene, RayArray[i] );
			pfnShadingFunction( pScene, RayArray[i], Hit, 2, &RenderedColor );
			outColorArray[i] = RenderedColor.ToVector3();
		}
		return;
	}

	// thread
	RaytracerProcedure *pRaytracerProcedureArray;
	pRaytracerProcedureArray = new RaytracerProcedure[ThreadCount];

	unsigned int RayCountPerThread = RayCount/ThreadCount;
	int RestRayCount = (int(RayCount) - int(RayCountPerThread * ThreadCount));
	assert( 0 <= RestRayCount );

	RaytracerProcedure::mRtScene = pScene;
	RaytracerProcedure::mKDTree = pKDTree;

	// TODO: MAX_THREAD?
	LPDWORD pThreadIdArray = new DWORD[ThreadCount];
	LPHANDLE pThreadArray = new HANDLE[ThreadCount];

	unsigned int RayIndex = 0;
	for( unsigned int i = 0; i < ThreadCount; i++ )
	{
		RaytracerProcedure *pRaytracerProcedure = &((RaytracerProcedure*)pRaytracerProcedureArray)[i];

		pRaytracerProcedure->mRayArray = &RayArray[RayIndex];
		pRaytracerProcedure->mRayCount = RayCountPerThread;
		pRaytracerProcedure->mOutData = &outColorArray[RayIndex];
		pRaytracerProcedure->mShadingFunction = pfnShadingFunction;

		pThreadArray[i] = CreateThread( 
			NULL,							// default security attributes
			0,								// use default stack size  
			RaytracerProcedure::UserDefinedShadingProcedure,       // thread function name
			//NULL, 
			(LPVOID)pRaytracerProcedure,  // argument to thread function 
			CREATE_SUSPENDED,
			//NULL );
			&pThreadIdArray[i] );			// returns the thread identifier 

		RayIndex+= RayCountPerThread;
	}

	RaytracerProcedure *pLastRaytracerProcedure = &((RaytracerProcedure*)&pRaytracerProcedureArray)[ThreadCount-1];
	//pLastRaytracerProcedure->mRayCount += RestRayCount;

	for( unsigned int i = 0; i < ThreadCount; i++ )
		ResumeThread( pThreadArray[i] );

	WaitForMultipleObjects( ThreadCount, pThreadArray, TRUE, INFINITE );

	for( unsigned int i = 0; i < ThreadCount; i++ )
		CloseHandle( pThreadArray[i] );

	delete[] pThreadArray;
	delete[] pThreadIdArray;
	delete[] pRaytracerProcedureArray;
}

GIVector3 GIEngine::CalcIrradiance( GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, unsigned int RayCount, unsigned int ThreadCount )
{
	
	assert( Scene );

	SphericalSampleArray SampleArray( RayCount, SphericalSampleArray::SA_RAY_ARRAY|SphericalSampleArray::SA_WEIGHT_ARRAY );
	assert( SampleArray.GetRayArray() != NULL );
	assert( SampleArray.GetWeightArray() != NULL );	

	GenerateHemisphericalSampleArray( Position, Normal, &SampleArray );

	GIVector3 *ColorArray = new GIVector3[RayCount];
	SampleColor( ThreadCount, Scene, RayCount, SampleArray.GetRayArray(), ColorArray );
	float *WeightArray = SampleArray.GetWeightArray();
	
	float recRayCount = 1.0f/(float(RayCount));
	GIVector3 ReturnColor( 0.0f, 0.0f, 0.0f );
	for( unsigned int i = 0; i < RayCount; i++ )
	{
		ReturnColor += ColorArray[i] * WeightArray[i];
	}
	ReturnColor *= 2.0f * recRayCount; // * FLOAT_PI /FLOAT_PI
	
	SafeDeleteArray( &ColorArray );

	return ReturnColor;
}

void GIEngine::CalcAmbientCubeVolume( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *pScene, AmbientCubeIrradianceVolume *AmbientCubeVolume )
{
	for( int z = 0; z < AmbientCubeVolume->GetSizeZ(); z++ )
		for( int y = 0; y < AmbientCubeVolume->GetSizeY(); y++ )
			for( int x = 0; x < AmbientCubeVolume->GetSizeX(); x++ )
	{
		GIVector3 Position = AmbientCubeVolume->GetPosition( x, y, z );
		
		CalcAmbientCube( SamplingCount, ThreadCount, pScene, Position, &AmbientCubeVolume->GetAmbientCube( AmbientCubeVolume->GetIndex( x, y, z ) ) );
	}
}

void GIEngine::CalcAmbientCube( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, AmbientCube *ouAmbientCube )
{
	assert( Scene );
	
	assert( ouAmbientCube );

	ouAmbientCube->SetPosX( CalcIrradiance( Scene, Position, GIVector3( 1.0f, 0.0f, 0.0f ), SamplingCount, ThreadCount ) );
	ouAmbientCube->SetPosY( CalcIrradiance( Scene, Position, GIVector3( 0.0f, 1.0f, 0.0f ), SamplingCount, ThreadCount ) );
	ouAmbientCube->SetPosZ( CalcIrradiance( Scene, Position, GIVector3( 0.0f, 0.0f, 1.0f ), SamplingCount, ThreadCount ) );

	ouAmbientCube->SetNegX( CalcIrradiance( Scene, Position, GIVector3( -1.0f, 0.0f, 0.0f ), SamplingCount, ThreadCount ) );
	ouAmbientCube->SetNegY( CalcIrradiance( Scene, Position, GIVector3( 0.0f, -1.0f, 0.0f ), SamplingCount, ThreadCount ) );
	ouAmbientCube->SetNegZ( CalcIrradiance( Scene, Position, GIVector3( 0.0f, 0.0f, -1.0f ), SamplingCount, ThreadCount ) );
}

//template void GIEngine::CalcInterrefelctionSH<1>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<1> *outSHRed, SphericalHarmonics<1> *outSHGreen, SphericalHarmonics<1> *outSHBlue );
//template void GIEngine::CalcInterrefelctionSH<2>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<2> *outSHRed, SphericalHarmonics<2> *outSHGreen, SphericalHarmonics<2> *outSHBlue );
//template void GIEngine::CalcInterrefelctionSH<3>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<3> *outSHRed, SphericalHarmonics<3> *outSHGreen, SphericalHarmonics<3> *outSHBlue );
//template void GIEngine::CalcInterrefelctionSH<4>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<4> *outSHRed, SphericalHarmonics<4> *outSHGreen, SphericalHarmonics<4> *outSHBlue );
//template void GIEngine::CalcInterrefelctionSH<5>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<5> *outSHRed, SphericalHarmonics<5> *outSHGreen, SphericalHarmonics<5> *outSHBlue );
//template void GIEngine::CalcInterrefelctionSH<6>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<6> *outSHRed, SphericalHarmonics<6> *outSHGreen, SphericalHarmonics<6> *outSHBlue );

//template<unsigned int order>
//void GIEngine::CalcInterrefelctionSH( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, 
//						   SphericalHarmonics<order> *outSHRed, SphericalHarmonics<order> *outSHGreen, SphericalHarmonics<order> *outSHBlue )
//{
//	assert( outSH != NULL && outSH->GetOrder() < 6 );
//
//	const unsigned int &RayCount = SamplingCount;
//	GIHit *SampledHitArray = new GIHit[RayCount];
//
//	SphericalSampleArray SampleArray( SamplingCount, true, true );
//	GenerateSphericalSampleArray( Position, &SampleArray );
//	assert( SampleArray.GetThetaArray() != NULL );
//	assert( SampleArray.GetPhiArray() != NULL );
//
//	assert( SampleArray.GetWeightArray() );
//	for( unsigned int i = 0; i < SamplingCount; i++ )
//	{
//		float dot = GIVector3( SampleArray.GetRayArray()[i].dir ).DotProduct( Normal );;
//		SampleArray.GetWeightArray()[i] = dot>0.0f?dot:0.0f;
//	}
//
//	SampleHit( ThreadCount, Scene, SampleArray.GetRayArray(), RayCount, SampledHitArray );
//
//	const float WeightFactor = 4.0f * FLOAT_PI / float(SamplingCount);
//
//	int l = 0;
//	int m = 0;
//	for( unsigned int sh_i = 0; sh_i < outSH->GetNumberOfCoefficients(); sh_i++ )
//	{
//		float coefficient = 0.0f;
//		for( unsigned int ray = 0; ray < SamplingCount; ray++ )
//		{
//			float sh_value = SphericalHarmonics<order>::SH( l, m, SampleArray.GetThetaArray()[ray], SampleArray.GetPhiArray()[ray] );
//			float visibility = (SampledDistanceArray[ray]>=0.0f?0.0f:1.0f);
//			float weight = SampleArray.GetWeightArray()[ray];
//			/*if( weight == 0.0f )
//				visibility = 0.0f;*/
//			coefficient += sh_value * visibility;
//		}
//		coefficient *= WeightFactor;
//
//		outSH->coefficients[sh_i] = coefficient;
//
//		if( ++m > l )
//		{
//			l++;
//			m = -l;
//		}
//	}
//}

//void GIEngine::CalcAmbientOcclusion( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const float LimitDistance, float *outAmbientOcclusion )
//{
//	
//}

// TODO: 없앰
void GIEngine::SetRayShootEpsilon( float epsilon )
{
	gRayShootEpsilon = epsilon;
}

float GIEngine::GetRayShootEpsilon()
{
	return gRayShootEpsilon;
}
