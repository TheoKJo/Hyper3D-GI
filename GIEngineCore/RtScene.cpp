#include "GIEngineCorePCH.h"

#include "RtScene.h"

static const unsigned int modulo[] = {0,1,2,0,1};

void TriangleToTriAceel( GITriangle &Triangle, RtTriAccel &TriAccel )
{
	// triNum
	assert( Triangle.TriangleNumber != -1 );
	TriAccel.triNum = Triangle.TriangleNumber;

	const GIVertexGroup &vg0 = Triangle.vg0;
	const GIVertexGroup &vg1 = Triangle.vg1;
	const GIVertexGroup &vg2 = Triangle.vg2;

	/*float Au = vg0.Vertex.array[v]/vg0.Vertex.array[u];
	float Av = vg0.Vertex.array[v]/vg0.Vertex.array[v];*/

	const GIVector3 b = vg2.Vertex - vg0.Vertex;
	const GIVector3 c = vg1.Vertex - vg0.Vertex;

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
	TriAccel.n_d = TriangleNormal.DotProduct( vg0.Vertex )/TriangleNormal.array[k]; //!<??, negative?
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
	TriAccel.b_d = (b.array[v] * vg0.Vertex.array[u] - b.array[u] * vg0.Vertex.array[v])/denom;

	// gamma
	TriAccel.c_nu = c.array[v]/denom;
	TriAccel.c_nv = -c.array[u]/denom;
	TriAccel.c_d = (c.array[u] * vg0.Vertex.array[v] - c.array[v] * vg0.Vertex.array[u])/denom;
}

RtScene::RtScene()
	: mTriangleCount(0), mTriangleList(NULL), mTriAccelList(NULL), mAccelStructure(NULL), 
	mVertexCount(0), mVertexPositionList(NULL), mVertexNormalList(NULL), 
	mSHList(NULL), mSH_RGBList(NULL), 
	mLightCount(0), mLightArray(NULL), 
	mMaterialCount(0), mMaterialArray(NULL), 
	mBackgroundColor(0.0f, 0.0f, 0.0f, 1.0f ), 
	mSceneEpsilon(0.01f)
{
	mSceneName[0] = 0;
}

RtScene::~RtScene()
{
	Destroy();
}

void RtScene::SetName( const char *SceneName )
{
	strcpy_s( mSceneName, 255, SceneName );
}

const char* RtScene::GetName()
{
	//strcpy_s( mSceneName, 255, SceneName );
	return mSceneName;
}

void RtScene::InitializeTriangle( unsigned int TriangleCount )
{
	assert( TriangleCount > 0 );
	mTriangleCount = TriangleCount;
	mTriangleList = new GITriangle[mTriangleCount];
	mTriAccelList = new RtTriAccel[mTriangleCount];

	// TODO : 초기화
}

void RtScene::InitializeVertex( unsigned int VertexCount )
{
	assert( VertexCount > 0 );
	mVertexCount = VertexCount;

	mVertexPositionList = new GIVector3[mVertexCount];
	mVertexNormalList = new GIVector3[mVertexCount];
}

void RtScene::InitializeTriangleSH( unsigned int order )
{
	assert( mVertexCount > 0 && mSHList == NULL );
	switch( order )
	{
	case 1:
		mSHList = new SphericalHarmonics<1>[mVertexCount];
		break;
	case 2:
		mSHList = new SphericalHarmonics<2>[mVertexCount];
		break;
	case 3:
		mSHList = new SphericalHarmonics<3>[mVertexCount];
		break;
	case 4:
		mSHList = new SphericalHarmonics<4>[mVertexCount];
		break;
	case 5:
		mSHList = new SphericalHarmonics<5>[mVertexCount];
		break;
	case 6:
		mSHList = new SphericalHarmonics<6>[mVertexCount];
		break;
	}
}

void RtScene::InitializeTriangleSH_RGB( unsigned int order )
{
	assert( mVertexCount > 0 && mSH_RGBList == NULL );
	switch( order )
	{
	case 1:
		mSH_RGBList = new SphericalHarmonicsRGB<1>[mVertexCount];
		break;
	case 2:
		mSH_RGBList = new SphericalHarmonicsRGB<2>[mVertexCount];
		break;
	case 3:
		mSH_RGBList = new SphericalHarmonicsRGB<3>[mVertexCount];
		break;
	case 4:
		mSH_RGBList = new SphericalHarmonicsRGB<4>[mVertexCount];
		break;
	case 5:
		mSH_RGBList = new SphericalHarmonicsRGB<5>[mVertexCount];
		break;
	case 6:
		mSH_RGBList = new SphericalHarmonicsRGB<6>[mVertexCount];
		break;
	}
}

void RtScene::InitializeLight( unsigned int LightCount )
{
	if( LightCount == 0 )
		return;
	assert( mLightArray == NULL );
	mLightCount = LightCount;
	mLightArray = new GILight*[LightCount];

	for( unsigned int i = 0; i < LightCount; i++ )
		mLightArray[i] = NULL;
}

void RtScene::InitializeMaterial( unsigned int MaterialCount )
{
	if( MaterialCount == 0 )
		return;
	assert( mMaterialArray == NULL );
	mMaterialCount = MaterialCount;
	mMaterialArray = new GIMaterial*[MaterialCount];

	for( unsigned int i = 0; i < MaterialCount; i++ )
		mMaterialArray[i] = NULL;
}

GIMaterial* RtScene::FindMaterial( const char *strMaterialName )
{
	int foundIndex = -1;
	for( unsigned int i = 0; i < mMaterialCount; i++ )
	{
		if( mMaterialArray[i] != NULL &&
			strcmp( mMaterialArray[i]->GetMaterialName(), strMaterialName ) == 0 )
		{
			return mMaterialArray[i];
		}
	}
	return NULL;
}

int RtScene::FindMaterialIndex( const char *strMaterialName )
{
	for( unsigned int i = 0; i < mMaterialCount; i++ )
	{
		if( mMaterialArray[i] != NULL &&
			strcmp( mMaterialArray[i]->GetMaterialName(), strMaterialName ) == 0 )
		{
			return i;
		}
	}
	return -1;
}

void RtScene::SetAccStructure( RtAccelStructure *AccelStructure )
{
	mAccelStructure = AccelStructure;
}

RtAccelStructure* RtScene::GetAccStructure()
{
	return mAccelStructure;
}

void RtScene::ConvertAll()
{
	for( unsigned int i = 0; i < mTriangleCount; i++ )
	{
		mTriangleList[i].TriangleNumber = i;
		TriangleToTriAceel( mTriangleList[i], mTriAccelList[i] );
	}
	EvaluateBoundingBox();
}

void RtScene::SetBackgroundColor( const GIVector4 &BackgroundColor )
{
	mBackgroundColor = BackgroundColor;
}

const GIVector4& RtScene::GetBackgroundColor()
{
	return mBackgroundColor;
}

void RtScene::SetSceneEpsilon( float Epslion )
{
	mSceneEpsilon = Epslion;
}

float RtScene::GetSceneEpsilon()
{
	return mSceneEpsilon;
}


void RtScene::EvaluateBoundingBox()
{
	GIVector3 MinPositions = GIVector3( 1e15f, 1e15f, 1e15f );
	GIVector3 MaxPositions = GIVector3( -1e15f, -1e15f, -1e15f );

	for( unsigned int i = 0; i < mTriangleCount; i++ )
	{
		GITriangle *Triangle = &mTriangleList[i];
		for( int Axis = 0; Axis < 3; Axis++ )
		{
			if( Triangle->vg0.Vertex.array[Axis] < MinPositions.array[Axis] )
				MinPositions.array[Axis] = Triangle->vg0.Vertex.array[Axis];
			if( Triangle->vg1.Vertex.array[Axis] < MinPositions.array[Axis] )
				MinPositions.array[Axis] = Triangle->vg1.Vertex.array[Axis];
			if( Triangle->vg2.Vertex.array[Axis] < MinPositions.array[Axis] )
				MinPositions.array[Axis] = Triangle->vg2.Vertex.array[Axis];

			if( MaxPositions.array[Axis] < Triangle->vg0.Vertex.array[Axis] )
				MaxPositions.array[Axis] = Triangle->vg0.Vertex.array[Axis];
			if( MaxPositions.array[Axis] < Triangle->vg1.Vertex.array[Axis] )
				MaxPositions.array[Axis] = Triangle->vg1.Vertex.array[Axis];
			if( MaxPositions.array[Axis] < Triangle->vg2.Vertex.array[Axis] )
				MaxPositions.array[Axis] = Triangle->vg2.Vertex.array[Axis];
		}
	}

	mBoundingBox.MinPositions = MinPositions;
	mBoundingBox.MaxPositions = MaxPositions;
}

GIBoundingBox RtScene::GetBoundingBox()
{
	return mBoundingBox;
}

GIVector3 RtScene::GetPosition( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < mTriangleCount );
	GITriangle &Triangle = mTriangleList[TriangleNum];

	return Triangle.vg0.Vertex * (1.0f - u - v) + 
		Triangle.vg1.Vertex * u + 
		Triangle.vg2.Vertex * v;
}

GIVector4 RtScene::GetColor( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < mTriangleCount );
	GITriangle &Triangle = mTriangleList[TriangleNum];

	return Triangle.vg0.Color * (1.0f - u - v) + 
		Triangle.vg1.Color * u + 
		Triangle.vg2.Color * v;
}

GIVector3 RtScene::GetNormal( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < mTriangleCount );
	GITriangle &Triangle = mTriangleList[TriangleNum];
	assert( Triangle.HasNormal() );

	return Triangle.vg0.Normal * (1.0f - u - v) + 
		Triangle.vg1.Normal * u + 
		Triangle.vg2.Normal * v;
}

GIVector3 RtScene::GetTangent( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < mTriangleCount );
	GITriangle &Triangle = mTriangleList[TriangleNum];
	assert( Triangle.HasTangent() );

	return Triangle.vg0.Tangent * (1.0f - u - v) + 
		Triangle.vg1.Tangent * u + 
		Triangle.vg2.Tangent * v;
}

GIVector2 RtScene::GetTexCoords( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < mTriangleCount );
	GITriangle &Triangle = mTriangleList[TriangleNum];

	return Triangle.vg0.TexCoords * (1.0f - u - v) + 
		Triangle.vg1.TexCoords * u + 
		Triangle.vg2.TexCoords * v;
}

GIVector4 RtScene::GetAlbedo( unsigned int TriangleNum, float u, float v )
{
	assert( 0 <= TriangleNum && TriangleNum < mTriangleCount );
	GITriangle &Triangle = mTriangleList[TriangleNum];

	if( Triangle.HasMaterial() )
	{
		GIVector2 TexCoord = Triangle.vg0.TexCoords * (1.0f - u - v) + 
			Triangle.vg1.TexCoords * u + 
			Triangle.vg2.TexCoords * v;

		GIMaterial *pMaterial = mMaterialArray[Triangle.MaterialIndex];
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
	assert( 0 <= TriangleNum && TriangleNum < mTriangleCount );

	GITriangle &Triangle = mTriangleList[TriangleNum];
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
	assert( 0 <= TriangleNum && TriangleNum < mTriangleCount );

	GITriangle &Triangle = mTriangleList[TriangleNum];

	return
		GetSH_RGBList<order>()[Triangle.vg0.VertexNum] * (1.0f - u - v) + 
		GetSH_RGBList<order>()[Triangle.vg1.VertexNum] * u + 
		GetSH_RGBList<order>()[Triangle.vg2.VertexNum] * v;
}

void RtScene::Destroy()
{
	mTriangleCount = 0;
	SafeDeleteArray( &mTriangleList );
	SafeDeleteArray( &mTriAccelList );
	SafeDeleteArray( &mMaterialArray );
	SafeDeleteArray( &mLightArray );
	SafeDeleteArray( &mSHList );
	SafeDeleteArray( &mSH_RGBList );
}