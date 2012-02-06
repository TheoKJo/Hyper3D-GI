/*!
 * \file		GIScene.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:18
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "Structures.h"
#include "Light.h"
#include "Material.h"
#include "../GIEngine/SphericalHarmonics/SphericalHarmonics.h"

#include <vector>

// Convert Functions
//! Convert to Barycentric
static void TriangleToTriAceel( GITriangle &Triangle, GITriAccel &TriAccel );

/*!
 * Refine 된 삼각형들을 가지고 있음. (Array 구조.)
 * @warning 이곳에서 NIF->GITriangle 을 수행하지는 않음. (NIF의 존재를 모름)
 * @warning 렌더링시의 편의를 위해 모든 개수는 변경되지 않는다고 가정(즉, 삼각형, 조명, 재질 등을 추가/삭제할 수 없음).
 */
namespace GIEngine {
class GIScene
{
public:
	GIScene();
	virtual ~GIScene();

	void SetName( const char *SceneName );
	const char* GetName();

	void InitializeTriangle( unsigned int TriangleCount );
	void InitializeVertex( unsigned int VertexCount );
	void InitializeTriangleSH( unsigned int order );
	void InitializeTriangleSH_RGB( unsigned int order );
	void InitializeLight( unsigned int LightCount );
	void InitializeMaterial( unsigned int MaterialCount );

	bool SaveToFile( const char *filename );
	bool LoadFromFile( const char *filename );

	GIMaterial* FindMaterial( const char *strMaterialName );
	//! TODO: 이 함수는 안쓰이는게 좋음. 쓸 필요가 없으면 나중에 삭제.
	//! @return 찾은 Material의 Index, 찾는 material이 없으면 음수를 반환.
	int FindMaterialIndex( const char *strMaterialName );

	/*void SetAccStructure( RtAccelStructure *AccelStructure );
	RtAccelStructure* GetAccStructure();*/

	void ConvertAll();
	
	//int GetMaterialIndex( const char *strMaterialName );

	void SetBackgroundColor( const GIVector4 &BackgroundColor );
	const GIVector4& GetBackgroundColor();

	void SetSceneEpsilon( float Epslion );
	float GetSceneEpsilon();

private:
	void EvaluateBoundingBox();
public:
	GIBoundingBox GetBoundingBox();

	// (Refined) Triangles
	unsigned int GetTriangleCount() { return m_TriangleCount; }
	GITriangle GetTriangle( unsigned int TriangleIndex ) { return m_TriangleList[TriangleIndex]; }
	const GITriangle& GetTriangle( unsigned int TriangleIndex ) const { return m_TriangleList[TriangleIndex]; }
	GITriangle* GetTriangleList() { return m_TriangleList; }
	GITriAccel* GetTriAccelList() { return m_TriAccelList; }

	// Vertices (not-refined)
	unsigned int GetVertexCount() { return m_VertexCount; }
	GIVector3* GetVertexPositionList() { return m_VertexPositionList; }
	GIVector3* GetVertexNormalList() { return m_VertexNormalList; }

	GIVector3 GetPosition( unsigned int TriangleNum, float u, float v );
	GIVector4 GetColor( unsigned int TriangleNum, float u, float v );
	GIVector3 GetNormal( unsigned int TriangleNum, float u, float v );
	GIVector3 GetTangent( unsigned int TriangleNum, float u, float v );
	GIVector2 GetTexCoords( unsigned int TriangleNum, float u, float v );
	GIVector4 GetAlbedo( unsigned int TriangleNum, float u, float v );

	/*SphericalHarmonicsAbs* GetSHList() { return m_SHList; }
	SphericalHarmonicsAbsRGB* GetSH_RGBList() { m_SH_RGBList; }*/

	template<unsigned int order>
	SphericalHarmonics<order> GetSH( unsigned int TriangleNum, float u, float v );
	template<unsigned int order>
	SphericalHarmonicsRGB<order> GetSH_RGB( unsigned int TriangleNum, float u, float v );

	template<unsigned int order>
	SphericalHarmonics<order>* GetSHList()
	{
		if( m_SHList == NULL )
			return NULL;
		assert( order == m_SHList->GetOrder() );
		if( order != m_SHList->GetOrder() )
			return NULL;
		return (SphericalHarmonics<order>*)m_SHList;
	}
	template<unsigned int order>
	SphericalHarmonicsRGB<order>* GetSH_RGBList()
	{
		if( m_SH_RGBList == NULL )
			return NULL;
		assert( order == m_SH_RGBList->GetOrder() );
		if( order != m_SH_RGBList->GetOrder() )
			return NULL;
		return (SphericalHarmonicsRGB<order>*)m_SH_RGBList;
	}

	// Lights
	unsigned int GetLightCount() { return m_LightCount; }
	GILight* GetLight( unsigned int i ) { return m_LightArray[i]; }
	GILight** GetLightArray() { return m_LightArray; }

	// Materials
	unsigned int GetMaterialCount() { return m_MaterialCount; }
	GIMaterial** GetMaterialArray() { return m_MaterialArray; }

private:
	void Destroy();

private:
	char m_SceneName[255];
	unsigned int m_TriangleCount;
	GITriangle *m_TriangleList;
	GITriAccel *m_TriAccelList;

	// 
	unsigned int m_VertexCount;
	GIVector3 *m_VertexPositionList;
	GIVector3 *m_VertexNormalList;

	SphericalHarmonicsAbs *m_SHList;
	SphericalHarmonicsAbsRGB *m_SH_RGBList;

	GIBoundingBox m_BoundingBox;
	//RtAccelStructure *m_AccelStructure;

	unsigned int m_LightCount;
	GILight **m_LightArray;

	unsigned int m_MaterialCount;
	GIMaterial **m_MaterialArray;

	GIVector4 m_BackgroundColor;
	float m_SceneEpsilon;
};

template SphericalHarmonics<1> GIScene::GetSH<1>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<2> GIScene::GetSH<2>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<3> GIScene::GetSH<3>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<4> GIScene::GetSH<4>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<5> GIScene::GetSH<5>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<6> GIScene::GetSH<6>( unsigned int TriangleNum, float u, float v );

template SphericalHarmonicsRGB<1> GIScene::GetSH_RGB<1>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<2> GIScene::GetSH_RGB<2>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<3> GIScene::GetSH_RGB<3>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<4> GIScene::GetSH_RGB<4>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<5> GIScene::GetSH_RGB<5>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<6> GIScene::GetSH_RGB<6>( unsigned int TriangleNum, float u, float v );

};
//template void GIScene::InitializeTriangleSH<1>();
//template void GIScene::InitializeTriangleSH<2>();
//template void GIScene::InitializeTriangleSH<3>();
//template void GIScene::InitializeTriangleSH<4>();
//template void GIScene::InitializeTriangleSH<5>();
//template void GIScene::InitializeTriangleSH<6>();
//
//template void GIScene::InitializeTriangleSH_RGB<1>();
//template void GIScene::InitializeTriangleSH_RGB<2>();
//template void GIScene::InitializeTriangleSH_RGB<3>();
//template void GIScene::InitializeTriangleSH_RGB<4>();
//template void GIScene::InitializeTriangleSH_RGB<5>();
//template void GIScene::InitializeTriangleSH_RGB<6>();