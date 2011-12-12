/*!
 * \file		RadiosityScene.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

//#include "RadiosityLightMapMaterial.h"

namespace GIEngine {

class GIScene;

namespace Radiosity {

class RadiosityLightMapMaterial;

/*!
 * 
 */
class RadiosityScene
{
public:
	RadiosityScene();
	virtual ~RadiosityScene();

	bool Initialize( GIScene *pScene );
	GIScene* GetScene() { return m_Scene; }

	void InitializeQuadrangle( unsigned int QuadrangleCount );
	void InitializeVertex( unsigned int VertexCount );
	//void InitializeQuadrangleSH( unsigned int order );
	//void InitializeQuadrangleSH_RGB( unsigned int order );

public:
	bool InitializeFormFactor();
	bool IsFormFactorInitialize();
	void FinalizeFormFactor();
	void SetFormFactor( unsigned int from_QuadNum/*column*/, unsigned int toQuadNum/*row*/, float FormFactor );
	float GetFormFactor( unsigned int from_QuadNum/*column*/, unsigned int toQuadNum/*row*/ );

	bool BuildWavelet();

private:
	// row-major order
	unsigned int m_FormFactorCount;
	float *m_FormFactors;

	// wavelets

public:
	void ConvertAll();

public:
	// (Refined) Quadrangles
	unsigned int GetQuadrangleCount() { return m_QuadrangleCount; }
	GIQuadrangle GetQuadrangle( unsigned int QuadrangleIndex ) { return m_QuadrangleList[QuadrangleIndex]; }
	bool GetQuadrangle( unsigned int *outQuadrangleNum, int TriangleNum, float u, float v )
	{
		return false;
	}
	const GIQuadrangle& GetQuadrangle( unsigned int QuadrangleIndex ) const { return m_QuadrangleList[QuadrangleIndex]; }
	GIQuadrangle* GetQuadrangleList() { return m_QuadrangleList; }

	// Vertices (not-refined)
	unsigned int GetVertexCount() { return m_VertexCount; }
	GIVector3* GetVertexPositionList() { return m_VertexPositionList; }
	GIVector3* GetVertexNormalList() { return m_VertexNormalList; }

	/*GIVector3 GetPosition( unsigned int QuadrangleNum, float u, float v );
	GIVector4 GetColor( unsigned int QuadrangleNum, float u, float v );
	GIVector3 GetNormal( unsigned int QuadrangleNum, float u, float v );
	GIVector3 GetTangent( unsigned int QuadrangleNum, float u, float v );
	GIVector2 GetTexCoords( unsigned int QuadrangleNum, float u, float v );
	GIVector4 GetAlbedo( unsigned int QuadrangleNum, float u, float v );*/

private:
	void Destroy();

private:
	GIScene *m_Scene;
	unsigned int m_MaterialCount;
	// TODO: double pointer 인데 네이밍이 엉망
	RadiosityLightMapMaterial **m_LightMapMaterialList;


	unsigned int m_QuadrangleCount;
	GIQuadrangle *m_QuadrangleList; // 필요 없을지도?

	GIVector3 *m_QuadranglePositionList;
	GIVector3 *m_QuadrangleNormalList;
	GIColor3 *m_QuadrangleColorList;

	float *m_QuadrangleAreaList; // 사이즈? 비율?

	unsigned int m_VertexCount;
	GIVector3 *m_VertexPositionList;
	GIVector3 *m_VertexNormalList;

	/*SphericalHarmonicsAbs *m_SHList;
	SphericalHarmonicsAbsRGB *m_SH_RGBList;*/

};

};};