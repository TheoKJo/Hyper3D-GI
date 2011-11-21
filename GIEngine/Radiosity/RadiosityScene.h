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
	GIScene* GetScene() { return mScene; }

	void InitializeQuadrangle( unsigned int QuadrangleCount );
	void InitializeVertex( unsigned int VertexCount );
	//void InitializeQuadrangleSH( unsigned int order );
	//void InitializeQuadrangleSH_RGB( unsigned int order );

public:
	bool InitializeFormFactor();
	bool IsFormFactorInitialize();
	void FinalizeFormFactor();
	void SetFormFactor( unsigned int fromQuadNum/*column*/, unsigned int toQuadNum/*row*/, float FormFactor );
	float GetFormFactor( unsigned int fromQuadNum/*column*/, unsigned int toQuadNum/*row*/ );

	bool BuildWavelet();

private:
	// row-major order
	unsigned int mFormFactorCount;
	float *mFormFactors;

	// wavelets

public:
	void ConvertAll();

public:
	// (Refined) Quadrangles
	unsigned int GetQuadrangleCount() { return mQuadrangleCount; }
	GIQuadrangle GetQuadrangle( unsigned int QuadrangleIndex ) { return mQuadrangleList[QuadrangleIndex]; }
	bool GetQuadrangle( unsigned int *outQuadrangleNum, int TriangleNum, float u, float v );
	const GIQuadrangle& GetQuadrangle( unsigned int QuadrangleIndex ) const { return mQuadrangleList[QuadrangleIndex]; }
	GIQuadrangle* GetQuadrangleList() { return mQuadrangleList; }

	// Vertices (not-refined)
	unsigned int GetVertexCount() { return mVertexCount; }
	GIVector3* GetVertexPositionList() { return mVertexPositionList; }
	GIVector3* GetVertexNormalList() { return mVertexNormalList; }

	/*GIVector3 GetPosition( unsigned int QuadrangleNum, float u, float v );
	GIVector4 GetColor( unsigned int QuadrangleNum, float u, float v );
	GIVector3 GetNormal( unsigned int QuadrangleNum, float u, float v );
	GIVector3 GetTangent( unsigned int QuadrangleNum, float u, float v );
	GIVector2 GetTexCoords( unsigned int QuadrangleNum, float u, float v );
	GIVector4 GetAlbedo( unsigned int QuadrangleNum, float u, float v );*/

private:
	void Destroy();

private:
	GIScene *mScene;
	unsigned int mMaterialCount;
	// TODO: double pointer 인데 네이밍이 엉망
	RadiosityLightMapMaterial **mLightMapMaterialList;


	unsigned int mQuadrangleCount;
	GIQuadrangle *mQuadrangleList; // 필요 없을지도?

	GIVector3 *mQuadranglePositionList;
	GIVector3 *mQuadrangleNormalList;
	GIColor3 *mQuadrangleColorList;

	float *mQuadrangleAreaList; // 사이즈? 비율?

	unsigned int mVertexCount;
	GIVector3 *mVertexPositionList;
	GIVector3 *mVertexNormalList;

	/*SphericalHarmonicsAbs *mSHList;
	SphericalHarmonicsAbsRGB *mSH_RGBList;*/

};

};};