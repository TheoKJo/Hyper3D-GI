#include "GIEnginePCH.h"

#include "RadiosityScene.h"
#include "RadiosityLightMapMaterial.h"

#include <GIEngine/GIScene.h>

#include <assert.h>

using namespace GIEngine;
using namespace GIEngine::Radiosity;

RadiosityScene::RadiosityScene()
	: mScene(NULL)
{
}

RadiosityScene::~RadiosityScene()
{
	Destroy();
}

bool RadiosityScene::Initialize( GIScene *pScene )
{
	assert( pScene != NULL );

	mMaterialCount = pScene->GetMaterialCount();
	GIMaterial **MaterialArray = pScene->GetMaterialArray();
	SafeDeleteArray( &mLightMapMaterialList );
	mLightMapMaterialList = new RadiosityLightMapMaterial*[mMaterialCount];
	
	for( unsigned int i = 0; i < mMaterialCount; i++ )
	{
		GITexture *pTexture = MaterialArray[i]->GetAlbedoTexture();
		mLightMapMaterialList[i] = new RadiosityLightMapMaterial( pTexture->GetWidth(), pTexture->GetHeight() );
	}
	return true;
}

void RadiosityScene::InitializeQuadrangle( unsigned int QuadrangleCount )
{
	mQuadrangleCount = QuadrangleCount;
}

void RadiosityScene::InitializeVertex( unsigned int VertexCount )
{
}

//void RadiosityScene::InitializeQuadrangleSH( unsigned int order );
//void RadiosityScene::InitializeQuadrangleSH_RGB( unsigned int order );

bool RadiosityScene::InitializeFormFactor()
{
	if( mQuadrangleCount == 0 )
		return false;
	//mFormFactorCount = mQuadrangleCount * (1+mQuadrangleCount) / 2; // sum_{i=1}^n i, n = QuadrangleCount
	mFormFactorCount = mQuadrangleCount * mQuadrangleCount; // sum_{i=1}^n i, n = QuadrangleCount
	// row-major order
	mFormFactors = new float[mFormFactorCount];
	return true;
}

bool RadiosityScene::IsFormFactorInitialize()
{
	return mFormFactorCount > 0 && mFormFactors != NULL;
}

void RadiosityScene::FinalizeFormFactor()
{
	SafeDeleteArray( &mFormFactors );
	mFormFactorCount = 0;
}

void RadiosityScene::SetFormFactor( unsigned int fromQuadNum/*column*/, unsigned int toQuadNum/*row*/, float FormFactor )
{
	assert( mFormFactorCount > 0 );
	if( fromQuadNum < toQuadNum )
	{
		unsigned int temp = fromQuadNum;
		fromQuadNum = toQuadNum;
		toQuadNum = temp;
	}

	// row-major order
	// TODO: ASSERT
	mFormFactors[toQuadNum * mQuadrangleCount + fromQuadNum] = FormFactor;
}

float RadiosityScene::GetFormFactor( unsigned int fromQuadNum/*column*/, unsigned int toQuadNum/*row*/ )
{
	assert( mFormFactorCount > 0 );
	if( fromQuadNum < toQuadNum )
	{
		unsigned int temp = fromQuadNum;
		fromQuadNum = toQuadNum;
		toQuadNum = temp;
	}

	// row-major order
	// TODO: ASSERT
	return mFormFactors[toQuadNum * mQuadrangleCount + fromQuadNum];
}

bool RadiosityScene::BuildWavelet()
{
	bool BuildFormFactor = false;
	if( mFormFactorCount == 0 || mFormFactors == NULL )
	{
		if( InitializeFormFactor() == false )
			return false;
		BuildFormFactor = true;
	}

	// 이 함 수는 밖으로 갈듯?
	
	if( BuildFormFactor )
	{
		// FormFactors를 이 함수 안에서 만들었으면, 이 함수 안에서 삭제
		FinalizeFormFactor();
	}
	return true;
}	

//GIVector3 RadiosityScene::GetPosition( unsigned int QuadrangleNum, float u, float v )
//{
//	return 
//}
//
//GIVector4 RadiosityScene::GetColor( unsigned int QuadrangleNum, float u, float v )
//{
//}
//
//GIVector3 RadiosityScene::GetNormal( unsigned int QuadrangleNum, float u, float v )
//{
//}
//
//GIVector3 RadiosityScene::GetTangent( unsigned int QuadrangleNum, float u, float v )
//{
//}
//
//GIVector2 RadiosityScene::GetTexCoords( unsigned int QuadrangleNum, float u, float v )
//{
//}

void RadiosityScene::Destroy()
{
	FinalizeFormFactor();
}