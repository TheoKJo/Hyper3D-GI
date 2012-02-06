/*!
 * \file		RtScene.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:18
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEngineCorePCH.h"

#include "RtScene.h"

#include <fstream>

static const unsigned int modulo[] = {0,1,2,0,1};

void TriangleToTriAceel( GITriangle &Triangle, RtTriAccel &TriAccel )
{
	// triNum
	assert( Triangle.TriangleNumber != -1 );
	TriAccel.triNum = Triangle.TriangleNumber;

	const GIVertexGroup &vg0 = Triangle.vg0;
	const GIVertexGroup &vg1 = Triangle.vg1;
	const GIVertexGroup &vg2 = Triangle.vg2;

	/*float Au = vg0.Position.array[v]/vg0.Position.array[u];
	float Av = vg0.Position.array[v]/vg0.Position.array[v];*/

	const GIVector3 b = vg2.Position - vg0.Position;
	const GIVector3 c = vg1.Position - vg0.Position;

	GIVector3 TriangleNormal = c.CrossProduct( b );
	TriangleNormal = TriangleNormal.GetNormalized();

	//! k, projection dimension
	int k = -1;
	if( fabs( TriangleNormal.array[0]) > fabs(TriangleNormal.array[1]) )
		k = (fabs( TriangleNormal.array[0]) > fabs(TriangleNormal.array[2]) )?0:2;
	else
		k = (fabs( TriangleNormal.array[1]) > fabs(TriangleNormal.array[2]) )?1:2;
	assert( k != -1 );
	TriAccel.k = k;

	unsigned int u = modulo[k+1]; // u
	unsigned int v = modulo[k+2]; // v

	// n
	assert( TriangleNormal.array[k] != 0.0f );
	TriAccel.n_u = TriangleNormal.array[u]/TriangleNormal.array[k];
	TriAccel.n_v = TriangleNormal.array[v]/TriangleNormal.array[k];
	TriAccel.n_d = TriangleNormal.DotProduct( vg0.Position )/TriangleNormal.array[k]; //!<??, negative?
	//=> N'u = Nu/Nk, N'k = 1

	// beta
	float denom = (b.array[u] * c.array[v] - b.array[v] * c.array[u]);
	//assert( denom != 0.0f );
	/*if( fabs( denom ) <= 0.00001f )
	{
		TriAccel.b_nu = 0.0f; TriAccel.b_nv = 0.0f; TriAccel.b_d = 0.0f;
		TriAccel.c_nu = 0.0f; TriAccel.c_nv = 0.0f; TriAccel.c_d = 0.0f;
		return;
	}*/
	TriAccel.b_nu = -b.array[v]/denom;
	TriAccel.b_nv = b.array[u]/denom;
	TriAccel.b_d = (b.array[v] * vg0.Position.array[u] - b.array[u] * vg0.Position.array[v])/denom;

	// gamma
	TriAccel.c_nu = c.array[v]/denom;
	TriAccel.c_nv = -c.array[u]/denom;
	TriAccel.c_d = (c.array[u] * vg0.Position.array[v] - c.array[v] * vg0.Position.array[u])/denom;
}

RtScene::RtScene()
	: m_TriangleCount(0), m_TriangleList(NULL), m_TriAccelList(NULL), //m_AccelStructure(NULL), 
	m_VertexCount(0), m_VertexPositionList(NULL), m_VertexNormalList(NULL), 
	m_SHList(NULL), m_SH_RGBList(NULL), 
	m_LightCount(0), m_LightArray(NULL), 
	m_MaterialCount(0), m_MaterialArray(NULL), 
	m_BackgroundColor(0.0f, 0.0f, 0.0f, 1.0f ), 
	m_SceneEpsilon(0.01f)
{
	m_SceneName[0] = 0;
}

RtScene::~RtScene()
{
	Destroy();
}

void RtScene::SetName( const char *SceneName )
{
	strcpy_s( m_SceneName, 255, SceneName );
}

const char* RtScene::GetName()
{
	//strcpy_s( m_SceneName, 255, SceneName );
	return m_SceneName;
}

void RtScene::InitializeTriangle( unsigned int TriangleCount )
{
	assert( TriangleCount > 0 );
	m_TriangleCount = TriangleCount;
	m_TriangleList = new GITriangle[m_TriangleCount];
	m_TriAccelList = new RtTriAccel[m_TriangleCount];

	// TODO : 초기화
}

void RtScene::InitializeVertex( unsigned int VertexCount )
{
	assert( VertexCount > 0 );
	m_VertexCount = VertexCount;

	m_VertexPositionList = new GIVector3[m_VertexCount];
	m_VertexNormalList = new GIVector3[m_VertexCount];
}

void RtScene::InitializeTriangleSH( unsigned int order )
{
	assert( m_VertexCount > 0 && m_SHList == NULL );
	switch( order )
	{
	case 1:
		m_SHList = new SphericalHarmonics<1>[m_VertexCount];
		break;
	case 2:
		m_SHList = new SphericalHarmonics<2>[m_VertexCount];
		break;
	case 3:
		m_SHList = new SphericalHarmonics<3>[m_VertexCount];
		break;
	case 4:
		m_SHList = new SphericalHarmonics<4>[m_VertexCount];
		break;
	case 5:
		m_SHList = new SphericalHarmonics<5>[m_VertexCount];
		break;
	case 6:
		m_SHList = new SphericalHarmonics<6>[m_VertexCount];
		break;
	}
}

void RtScene::InitializeTriangleSH_RGB( unsigned int order )
{
	assert( m_VertexCount > 0 && m_SH_RGBList == NULL );
	switch( order )
	{
	case 1:
		m_SH_RGBList = new SphericalHarmonicsRGB<1>[m_VertexCount];
		break;
	case 2:
		m_SH_RGBList = new SphericalHarmonicsRGB<2>[m_VertexCount];
		break;
	case 3:
		m_SH_RGBList = new SphericalHarmonicsRGB<3>[m_VertexCount];
		break;
	case 4:
		m_SH_RGBList = new SphericalHarmonicsRGB<4>[m_VertexCount];
		break;
	case 5:
		m_SH_RGBList = new SphericalHarmonicsRGB<5>[m_VertexCount];
		break;
	case 6:
		m_SH_RGBList = new SphericalHarmonicsRGB<6>[m_VertexCount];
		break;
	}
}

void RtScene::InitializeLight( unsigned int LightCount )
{
	if( LightCount == 0 )
		return;
	assert( m_LightArray == NULL );
	m_LightCount = LightCount;
	m_LightArray = new GILight*[LightCount];

	for( unsigned int i = 0; i < LightCount; i++ )
		m_LightArray[i] = NULL;
}

void RtScene::InitializeMaterial( unsigned int MaterialCount )
{
	if( MaterialCount == 0 )
		return;
	assert( m_MaterialArray == NULL );
	m_MaterialCount = MaterialCount;
	m_MaterialArray = new GIMaterial*[MaterialCount];

	for( unsigned int i = 0; i < MaterialCount; i++ )
		m_MaterialArray[i] = NULL;
}

bool RtScene::SaveToFile( const char *filename )
{
	int memNULL = NULL;
	std::fstream fs( filename, std::ios::out | std::ios::binary );
	if( !fs.is_open() )
		return false;
	fs << m_SceneName;
	fs.write( (char*)&m_TriangleCount, sizeof(m_TriangleCount) );
	if( m_TriangleList == NULL )
	{
		// ERROR
		return false;
	}
	fs.write( (char*)m_TriangleList, sizeof(GITriangle) * m_TriangleCount );

	fs.write( (char*)&m_VertexCount, sizeof(m_VertexCount) );
	fs.write( (char*)&m_VertexPositionList, sizeof(GIVector3) * m_VertexCount );
	if( m_VertexNormalList == NULL )
		fs.write( (char*)&memNULL, sizeof(memNULL) );
	else
		fs.write( (char*)&m_VertexNormalList, sizeof(GIVector3) * m_VertexCount );

	if( m_SHList == NULL )
		fs.write( (char*)&memNULL, sizeof(memNULL) );
	else
		fs.write( (char*)&m_SHList, sizeof(SphericalHarmonicsAbs) * m_VertexCount );
	if( m_SH_RGBList == NULL )
		fs.write( (char*)&memNULL, sizeof(memNULL) );
	else
		fs.write( (char*)&m_SH_RGBList, sizeof(SphericalHarmonicsAbsRGB) * m_VertexCount );

	fs.write( (char*)&m_BoundingBox, sizeof(GIBoundingBox) );
	//RtAccelStructure *m_AccelStructure;

	fs.write( (char*)&m_LightCount, sizeof(m_TriangleCount) );
	// 현재는 다 point light라 가정.
	for( unsigned int i = 0; i < m_LightCount; i++ )
	{
		//m_LightArray[i]->ToBinaryDataStream( 
	}
	//fs
	return false;
	
	/*m_LightArray->
	GILight **m_LightArray;

	unsigned int m_MaterialCount;
	GIMaterial **m_MaterialArray;

	GIVector4 m_BackgroundColor;
	float m_SceneEpsilon*/;
	return true;
}

bool RtScene::LoadFromFile( const char *filename )
{
	return false;
}

GIMaterial* RtScene::FindMaterial( const char *strMaterialName )
{
	int foundIndex = -1;
	for( unsigned int i = 0; i < m_MaterialCount; i++ )
	{
		if( m_MaterialArray[i] != NULL &&
			strcmp( m_MaterialArray[i]->GetMaterialName(), strMaterialName ) == 0 )
		{
			return m_MaterialArray[i];
		}
	}
	return NULL;
}

int RtScene::FindMaterialIndex( const char *strMaterialName )
{
	for( unsigned int i = 0; i < m_MaterialCount; i++ )
	{
		if( m_MaterialArray[i] != NULL &&
			strcmp( m_MaterialArray[i]->GetMaterialName(), strMaterialName ) == 0 )
		{
			return i;
		}
	}
	return -1;
}

//void RtScene::SetAccStructure( RtAccelStructure *AccelStructure )
//{
//	m_AccelStructure = AccelStructure;
//}
//
//RtAccelStructure* RtScene::GetAccStructure()
//{
//	return m_AccelStructure;
//}

void RtScene::ConvertAll()
{
	for( unsigned int i = 0; i < m_TriangleCount; i++ )
	{
		m_TriangleList[i].TriangleNumber = i;
		TriangleToTriAceel( m_TriangleList[i], m_TriAccelList[i] );
	}
	EvaluateBoundingBox();
}

void RtScene::SetBackgroundColor( const GIVector4 &BackgroundColor )
{
	m_BackgroundColor = BackgroundColor;
}

const GIVector4& RtScene::GetBackgroundColor()
{
	return m_BackgroundColor;
}

void RtScene::SetSceneEpsilon( float Epslion )
{
	m_SceneEpsilon = Epslion;
}

float RtScene::GetSceneEpsilon()
{
	return m_SceneEpsilon;
}


void RtScene::EvaluateBoundingBox()
{
	GIVector3 MinPositions = GIVector3( 1e15f, 1e15f, 1e15f );
	GIVector3 MaxPositions = GIVector3( -1e15f, -1e15f, -1e15f );

	for( unsigned int i = 0; i < m_TriangleCount; i++ )
	{
		GITriangle *Triangle = &m_TriangleList[i];
		for( int Axis = 0; Axis < 3; Axis++ )
		{
			if( Triangle->vg0.Position.array[Axis] < MinPositions.array[Axis] )
				MinPositions.array[Axis] = Triangle->vg0.Position.array[Axis];
			if( Triangle->vg1.Position.array[Axis] < MinPositions.array[Axis] )
				MinPositions.array[Axis] = Triangle->vg1.Position.array[Axis];
			if( Triangle->vg2.Position.array[Axis] < MinPositions.array[Axis] )
				MinPositions.array[Axis] = Triangle->vg2.Position.array[Axis];

			if( MaxPositions.array[Axis] < Triangle->vg0.Position.array[Axis] )
				MaxPositions.array[Axis] = Triangle->vg0.Position.array[Axis];
			if( MaxPositions.array[Axis] < Triangle->vg1.Position.array[Axis] )
				MaxPositions.array[Axis] = Triangle->vg1.Position.array[Axis];
			if( MaxPositions.array[Axis] < Triangle->vg2.Position.array[Axis] )
				MaxPositions.array[Axis] = Triangle->vg2.Position.array[Axis];
		}
	}

	m_BoundingBox.MinPositions = MinPositions;
	m_BoundingBox.MaxPositions = MaxPositions;
}

GIBoundingBox RtScene::GetBoundingBox()
{
	return m_BoundingBox;
}

GIVector3 RtScene::GetPosition( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < m_TriangleCount );
	GITriangle &Triangle = m_TriangleList[TriangleNum];

	return Triangle.vg0.Position * (1.0f - u - v) + 
		Triangle.vg1.Position * u + 
		Triangle.vg2.Position * v;
}

GIVector4 RtScene::GetColor( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < m_TriangleCount );
	GITriangle &Triangle = m_TriangleList[TriangleNum];

	return Triangle.vg0.Color * (1.0f - u - v) + 
		Triangle.vg1.Color * u + 
		Triangle.vg2.Color * v;
}

GIVector3 RtScene::GetNormal( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < m_TriangleCount );
	GITriangle &Triangle = m_TriangleList[TriangleNum];
	assert( Triangle.HasNormal() );

	return Triangle.vg0.Normal * (1.0f - u - v) + 
		Triangle.vg1.Normal * u + 
		Triangle.vg2.Normal * v;
}

GIVector3 RtScene::GetTangent( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < m_TriangleCount );
	GITriangle &Triangle = m_TriangleList[TriangleNum];
	assert( Triangle.HasTangent() );

	return Triangle.vg0.Tangent * (1.0f - u - v) + 
		Triangle.vg1.Tangent * u + 
		Triangle.vg2.Tangent * v;
}

GIVector2 RtScene::GetTexCoords( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < m_TriangleCount );
	GITriangle &Triangle = m_TriangleList[TriangleNum];

	return Triangle.vg0.TexCoords * (1.0f - u - v) + 
		Triangle.vg1.TexCoords * u + 
		Triangle.vg2.TexCoords * v;
}

GIVector4 RtScene::GetAlbedo( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < m_TriangleCount );
	GITriangle &Triangle = m_TriangleList[TriangleNum];

	if( Triangle.HasMaterial() )
	{
		GIVector2 TexCoord = Triangle.vg0.TexCoords * (1.0f - u - v) + 
			Triangle.vg1.TexCoords * u + 
			Triangle.vg2.TexCoords * v;

		GIMaterial *pMaterial = m_MaterialArray[Triangle.MaterialIndex];
		if( pMaterial == NULL )
			return GIVector4( 0.0f, 0.0f, 0.0f, 0.0f );
		// TODO: Material 속 Texture 마다 (eg. texture instance?) 각기 다른 속성을 가지고 있어야함)
		GITexture *pTexture = pMaterial->GetAlbedoTexture();
		if( pTexture )
			return pTexture->GetColor( TexCoord, false );
		return pMaterial->GetDiffuseColor();
	}
	else
	{
		return Triangle.vg0.Color * (1.0f - u - v) + 
			Triangle.vg1.Color * u + 
			Triangle.vg2.Color * v;
	}
}

template<unsigned int order>
SphericalHarmonics<order> RtScene::GetSH( unsigned int TriangleNum, float u, float v )
{
	assert( GetSHList<order>() != NULL );
	assert( 0 <= TriangleNum && TriangleNum < m_TriangleCount );

	GITriangle &Triangle = m_TriangleList[TriangleNum];
	SphericalHarmonics<order> SH0 = GetSHList<order>()[Triangle.vg0.VertexNum];
	SphericalHarmonics<order> SH1 = GetSHList<order>()[Triangle.vg1.VertexNum];
	SphericalHarmonics<order> SH2 = GetSHList<order>()[Triangle.vg2.VertexNum];

	return
		GetSHList<order>()[Triangle.vg0.VertexNum] * (1.0f - u - v) + 
		GetSHList<order>()[Triangle.vg1.VertexNum] * u + 
		GetSHList<order>()[Triangle.vg2.VertexNum] * v;
}

template<unsigned int order>
SphericalHarmonicsRGB<order> RtScene::GetSH_RGB( unsigned int TriangleNum, float u, float v )
{
	assert( GetSH_RGBList<order>() != NULL );
	assert( 0 <= TriangleNum && TriangleNum < m_TriangleCount );

	GITriangle &Triangle = m_TriangleList[TriangleNum];

	return
		GetSH_RGBList<order>()[Triangle.vg0.VertexNum] * (1.0f - u - v) + 
		GetSH_RGBList<order>()[Triangle.vg1.VertexNum] * u + 
		GetSH_RGBList<order>()[Triangle.vg2.VertexNum] * v;
}

void RtScene::Destroy()
{
	m_TriangleCount = 0;
	SafeDeleteArray( &m_TriangleList );
	SafeDeleteArray( &m_TriAccelList );
	SafeDeleteArray( &m_MaterialArray );
	SafeDeleteArray( &m_LightArray );
	SafeDeleteArray( &m_SHList );
	SafeDeleteArray( &m_SH_RGBList );
}