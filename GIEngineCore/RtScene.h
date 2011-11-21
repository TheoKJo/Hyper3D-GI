#pragma once

#include "Structures.h"
#include "Light.h"
#include "Material.h"
#include "../GIEngine/SphericalHarmonics/SphericalHarmonics.h"

#include <vector>

// Convert Functions
//! Convert to Barycentric
static void TriangleToTriAceel( GITriangle &Triangle, RtTriAccel &TriAccel );

/*!
 * Refine 된 삼각형들을 가지고 있음. (Array 구조.)
 * @warning 이곳에서 NIF->GITriangle 을 수행하지는 않음. (NIF의 존재를 모름)
 * @warning 렌더링시의 편의를 위해 모든 개수는 변경되지 않는다고 가정(즉, 삼각형, 조명, 재질 등을 추가/삭제할 수 없음).
 */
class RtScene
{
public:
	RtScene();
	virtual ~RtScene();

	void SetName( const char *SceneName );
	const char* GetName();

	void InitializeTriangle( unsigned int TriangleCount );
	void InitializeVertex( unsigned int VertexCount );
	void InitializeTriangleSH( unsigned int order );
	void InitializeTriangleSH_RGB( unsigned int order );
	void InitializeLight( unsigned int LightCount );
	void InitializeMaterial( unsigned int MaterialCount );

	GIMaterial* FindMaterial( const char *strMaterialName );
	//! TODO: 이 함수는 안쓰이는게 좋음. 쓸 필요가 없으면 나중에 삭제.
	//! @return 찾은 Material의 Index, 찾는 material이 없으면 음수를 반환.
	int FindMaterialIndex( const char *strMaterialName );

	void SetAccStructure( RtAccelStructure *AccelStructure );
	RtAccelStructure* GetAccStructure();

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
	unsigned int GetTriangleCount() { return mTriangleCount; }
	GITriangle GetTriangle( unsigned int TriangleIndex ) { return mTriangleList[TriangleIndex]; }
	const GITriangle& GetTriangle( unsigned int TriangleIndex ) const { return mTriangleList[TriangleIndex]; }
	GITriangle* GetTriangleList() { return mTriangleList; }
	RtTriAccel* GetTriAccelList() { return mTriAccelList; }

	// Vertices (not-refined)
	unsigned int GetVertexCount() { return mVertexCount; }
	GIVector3* GetVertexPositionList() { return mVertexPositionList; }
	GIVector3* GetVertexNormalList() { return mVertexNormalList; }

	GIVector3 GetPosition( unsigned int TriangleNum, float u, float v );
	GIVector4 GetColor( unsigned int TriangleNum, float u, float v );
	GIVector3 GetNormal( unsigned int TriangleNum, float u, float v );
	GIVector3 GetTangent( unsigned int TriangleNum, float u, float v );
	GIVector2 GetTexCoords( unsigned int TriangleNum, float u, float v );
	GIVector4 GetAlbedo( unsigned int TriangleNum, float u, float v );

	/*SphericalHarmonicsAbs* GetSHList() { return mSHList; }
	SphericalHarmonicsAbsRGB* GetSH_RGBList() { mSH_RGBList; }*/

	template<unsigned int order>
	SphericalHarmonics<order> GetSH( unsigned int TriangleNum, float u, float v );
	template<unsigned int order>
	SphericalHarmonicsRGB<order> GetSH_RGB( unsigned int TriangleNum, float u, float v );

	template<unsigned int order>
	SphericalHarmonics<order>* GetSHList()
	{
		if( mSHList == NULL )
			return NULL;
		assert( order == mSHList->GetOrder() );
		if( order != mSHList->GetOrder() )
			return NULL;
		return (SphericalHarmonics<order>*)mSHList;
	}
	template<unsigned int order>
	SphericalHarmonicsRGB<order>* GetSH_RGBList()
	{
		if( mSH_RGBList == NULL )
			return NULL;
		assert( order == mSH_RGBList->GetOrder() );
		if( order != mSH_RGBList->GetOrder() )
			return NULL;
		return (SphericalHarmonicsRGB<order>*)mSH_RGBList;
	}

	// Lights
	unsigned int GetLightCount() { return mLightCount; }
	GILight* GetLight( unsigned int i ) { return mLightArray[i]; }
	GILight** GetLightArray() { return mLightArray; }

	// Materials
	unsigned int GetMaterialCount() { return mMaterialCount; }
	GIMaterial** GetMaterialArray() { return mMaterialArray; }

private:
	void Destroy();

private:
	char mSceneName[255];
	unsigned int mTriangleCount;
	GITriangle *mTriangleList;
	RtTriAccel *mTriAccelList;

	// 
	unsigned int mVertexCount;
	GIVector3 *mVertexPositionList;
	GIVector3 *mVertexNormalList;

	SphericalHarmonicsAbs *mSHList;
	SphericalHarmonicsAbsRGB *mSH_RGBList;

	GIBoundingBox mBoundingBox;
	RtAccelStructure *mAccelStructure;

	unsigned int mLightCount;
	GILight **mLightArray;

	unsigned int mMaterialCount;
	GIMaterial **mMaterialArray;

	GIVector4 mBackgroundColor;
	float mSceneEpsilon;
};

template SphericalHarmonics<1> RtScene::GetSH<1>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<2> RtScene::GetSH<2>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<3> RtScene::GetSH<3>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<4> RtScene::GetSH<4>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<5> RtScene::GetSH<5>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonics<6> RtScene::GetSH<6>( unsigned int TriangleNum, float u, float v );

template SphericalHarmonicsRGB<1> RtScene::GetSH_RGB<1>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<2> RtScene::GetSH_RGB<2>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<3> RtScene::GetSH_RGB<3>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<4> RtScene::GetSH_RGB<4>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<5> RtScene::GetSH_RGB<5>( unsigned int TriangleNum, float u, float v );
template SphericalHarmonicsRGB<6> RtScene::GetSH_RGB<6>( unsigned int TriangleNum, float u, float v );

//template void RtScene::InitializeTriangleSH<1>();
//template void RtScene::InitializeTriangleSH<2>();
//template void RtScene::InitializeTriangleSH<3>();
//template void RtScene::InitializeTriangleSH<4>();
//template void RtScene::InitializeTriangleSH<5>();
//template void RtScene::InitializeTriangleSH<6>();
//
//template void RtScene::InitializeTriangleSH_RGB<1>();
//template void RtScene::InitializeTriangleSH_RGB<2>();
//template void RtScene::InitializeTriangleSH_RGB<3>();
//template void RtScene::InitializeTriangleSH_RGB<4>();
//template void RtScene::InitializeTriangleSH_RGB<5>();
//template void RtScene::InitializeTriangleSH_RGB<6>();