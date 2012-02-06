/*!
 * \file		RadiosityScene.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include "RadiosityScene.h"
#include "RadiosityLightMapMaterial.h"

#include <GIEngine/GIScene.h>

#include <assert.h>

using namespace GIEngine;
using namespace GIEngine::Radiosity;

RadiosityScene::RadiosityScene()
	: m_Scene(NULL), m_MaterialCount(0), m_LightMapMaterialList(NULL), 
	m_QuadrangleCount(0), m_QuadrangleList(NULL), 

	m_QuadranglePositionList(NULL), m_QuadrangleNormalList(NULL), m_QuadrangleColorList(NULL), 
	m_QuadrangleAreaList(NULL), 

	m_FormFactorCount(NULL), m_FormFactors(NULL), 

	m_VertexCount(NULL), m_VertexPositionList(NULL), m_VertexNormalList(NULL)
{
}

RadiosityScene::~RadiosityScene()
{
	Destroy();
}

bool RadiosityScene::Initialize( GIScene *pScene )
{
	assert( pScene != NULL );

	m_MaterialCount = pScene->GetMaterialCount();
	GIMaterial **MaterialArray = pScene->GetMaterialArray();
	SafeDeleteArray( &m_LightMapMaterialList );
	m_LightMapMaterialList = new RadiosityLightMapMaterial*[m_MaterialCount];
	
	for( unsigned int i = 0; i < m_MaterialCount; i++ )
	{
		GITexture *pTexture = MaterialArray[i]->GetAlbedoTexture();
		if( pTexture == NULL )
			m_LightMapMaterialList[i] = NULL;
		else
			m_LightMapMaterialList[i] = new RadiosityLightMapMaterial( pTexture->GetWidth(), pTexture->GetHeight() );
	}
	return true;
}

void RadiosityScene::InitializeQuadrangle( unsigned int QuadrangleCount )
{
	assert( m_QuadrangleCount == 0 );
	m_QuadrangleCount = QuadrangleCount;
}

void RadiosityScene::InitializeVertex( unsigned int VertexCount )
{
}

//void RadiosityScene::InitializeQuadrangleSH( unsigned int order );
//void RadiosityScene::InitializeQuadrangleSH_RGB( unsigned int order );

bool RadiosityScene::InitializeFormFactor()
{
	if( m_QuadrangleCount == 0 )
		return false;
	//m_FormFactorCount = m_QuadrangleCount * (1+m_QuadrangleCount) / 2; // sum_{i=1}^n i, n = QuadrangleCount
	m_FormFactorCount = m_QuadrangleCount * m_QuadrangleCount; // sum_{i=1}^n i, n = QuadrangleCount
	// row-major order
	m_FormFactors = new float[m_FormFactorCount];
	return true;
}

bool RadiosityScene::IsFormFactorInitialize()
{
	return m_FormFactorCount > 0 && m_FormFactors != NULL;
}

void RadiosityScene::FinalizeFormFactor()
{
	SafeDeleteArray( &m_FormFactors );
	m_FormFactorCount = 0;
}

void RadiosityScene::SetFormFactor( unsigned int from_QuadNum/*column*/, unsigned int toQuadNum/*row*/, float FormFactor )
{
	assert( m_FormFactorCount > 0 );
	if( from_QuadNum < toQuadNum )
	{
		unsigned int temp = from_QuadNum;
		from_QuadNum = toQuadNum;
		toQuadNum = temp;
	}

	// row-major order
	// TODO: ASSERT
	m_FormFactors[toQuadNum * m_QuadrangleCount + from_QuadNum] = FormFactor;
}

float RadiosityScene::GetFormFactor( unsigned int from_QuadNum/*column*/, unsigned int toQuadNum/*row*/ )
{
	assert( m_FormFactorCount > 0 );
	if( from_QuadNum < toQuadNum )
	{
		unsigned int temp = from_QuadNum;
		from_QuadNum = toQuadNum;
		toQuadNum = temp;
	}

	// row-major order
	// TODO: ASSERT
	return m_FormFactors[toQuadNum * m_QuadrangleCount + from_QuadNum];
}

bool RadiosityScene::BuildWavelet()
{
	bool BuildFormFactor = false;
	if( m_FormFactorCount == 0 || m_FormFactors == NULL )
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