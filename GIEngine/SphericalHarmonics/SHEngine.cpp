/*!
 * \file		SHEngine.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include "SHEngine.h"
#include "../GIEngine.h"

using namespace GIEngine;

template<unsigned int order>
void SHEngine::GenerateSphericalHarmonicsSampleArray( const GIVector3 &Position, SphericalHarmonicsSampleArray<order> *inoutSHSampleArray )
{
	GenerateSphericalSampleArray( Position, inoutSHSampleArray );
	inoutSHSampleArray->GetSphericalHArmonicsArray()
	
}

template<unsigned int order>
void SHEngine::GenerateSphericalHarmonicsSampleArray( const GIVector3 &Position, const GIVector3 &Normal, SphericalHarmonicsSampleArray<order> *inoutSHSampleArray )
{
}

// order3, identity matrix
static float identity[] =
{
	1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 
};

// order3, rotation -90 degree about x
static float X3_minus90[] =
{
	1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f, 
	0.0f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 
	0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, -0.5f, 0.0f, -0.866f, 
	0.0f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, -0.866f, 0.0f, 0.5f
};

// order3, rotation +90 degree about x
static float X3_plus90[] =
{
	1.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f,  0.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 
	0.0f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 
	0.0f,  0.0f, 0.0f, 0.0f, 0.0f,  0.0f, -0.5f, 0.0f, -0.866f, 
	0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, -0.866f, 0.0f, 0.5f
};

static const SHMatrix<3,3> IdentityMatrix33( identity );
static SHMatrix<3,3> RotationMatrix33_X_m90( X3_minus90 );
static SHMatrix<3,3> RotationMatrix33_X_p90( X3_plus90 );

static SHMatrix<3,3> RotationMatrix33_Z( float angle )
{
	SHMatrix<3,3> retMatrix( IdentityMatrix33 );

	float sin_a = sin( angle );
	float sin_2a = sin( 2.0f*angle );
	float cos_a = cos( angle );
	float cos_2a = cos( 2.0f*angle );

	retMatrix.SetElement( 1, 1, cos_a );  retMatrix.SetElement( 1, 3, sin_a );
	retMatrix.SetElement( 3, 1, -sin_a ); retMatrix.SetElement( 3, 3, cos_a );

	retMatrix.SetElement( 4, 4, cos_2a );	retMatrix.SetElement( 4, 8, sin_2a );
	retMatrix.SetElement( 5, 5, cos_a );	retMatrix.SetElement( 5, 7, sin_a );
	retMatrix.SetElement( 7, 5, -sin_a );	retMatrix.SetElement( 7, 7, cos_a );
	retMatrix.SetElement( 8, 4, -sin_2a );	retMatrix.SetElement( 8, 8, cos_2a );
	return retMatrix;
}

template<unsigned int order>
SphericalHarmonics<order>* SHEngine::CreateCircleLight( float radius )
{
	// TODO: option manager?
	const unsigned int CREATING_LIGHT_SAMPLE_COUNT = 1000;

	SphericalHarmonics<order> *SHLight = new SphericalHarmonics<order>;

	SphericalSampleArray SampleArray( CREATING_LIGHT_SAMPLE_COUNT, SphericalSampleArray::SA_THETA_ARRAY|SphericalSampleArray::SA_PHI_ARRAY );
	GenerateSphericalSampleArray( GIVector3( 0.0f, 0.0f, 0.0f ), &SampleArray );
	assert( SampleArray.GetThetaArray() != NULL );
	assert( SampleArray.GetPhiArray() != NULL );

	float WeightFactor = 4.0f * FLOAT_PI / float(CREATING_LIGHT_SAMPLE_COUNT);

	int l = 0;
	int m = 0;
	for( unsigned int sh_i = 0; sh_i < SHLight->GetNumberOfCoefficients(); sh_i++ )
	{
		float coefficient = 0.0f;
		for( unsigned int ray = 0; ray < CREATING_LIGHT_SAMPLE_COUNT; ray++ )
		{
			float theta = SampleArray.GetThetaArray()[ray];
			float phi = SampleArray.GetPhiArray()[ray];

			if( fabs( theta ) <= radius )
				coefficient += SphericalHarmonics<order>::SH( l, m, theta, phi );
		}

		coefficient *= WeightFactor;
		SHLight->coefficients[sh_i] = coefficient;

		if( ++m > l )
		{
			l++;
			m = -l;
		}
	}
	return SHLight;
}

SHMatrix<3, 3> SHEngine::MakeSHRotateMatrix33( float alpha, float beta, float gamma )
{
	return RotationMatrix33_Z( gamma ) * RotationMatrix33_X_m90 * RotationMatrix33_Z( beta ) * RotationMatrix33_X_p90 * RotationMatrix33_Z( alpha ) ;
}

template<unsigned int order>
void SHEngine::CalcSH_RGB( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, 
						  SphericalHarmonics<order> *outSHRed, SphericalHarmonics<order> *outSHGreen, SphericalHarmonics<order> *outSHBlue )
{
	assert( outSHRed->GetOrder() == outSHBlue->GetOrder() && outSHBlue->GetOrder() == outSHGreen->GetOrder() );
	assert( outSHRed->GetOrder() < 6 );

	const unsigned int &RayCount = SamplingCount;
	GIVector3 *RenderedColor = new GIVector3[RayCount];

	SphericalSampleArray SampleArray( RayCount, SphericalSampleArray::SA_RAY_ARRAY|SphericalSampleArray::SA_THETA_ARRAY|SphericalSampleArray::SA_PHI_ARRAY );
	GenerateSphericalSampleArray( Position, &SampleArray );
	assert( SampleArray.GetRayArray() != NULL );
	assert( SampleArray.GetThetaArray() != NULL );
	assert( SampleArray.GetPhiArray() != NULL );	

	// TODO: 0.5는 없애고, 제대로 된 조명을 넣어야함.(albedo를 낮춘셈)
	float WeightFactor = 4.0f * FLOAT_PI / float(RayCount) * 0.5f;

	// TODO: 샘플링 미리 계산

	// TODO: Coordinate 확인???

	// Depth 설정
	SampleColor( ThreadCount, Scene, RayCount, SampleArray.GetRayArray(), RenderedColor );

	int l = 0;
	int m = 0;
	for( unsigned int sh_i = 0; sh_i < outSHRed->GetNumberOfCoefficients(); sh_i++ )
	{
		float coefficient[3] = { 0.0f, 0.0f, 0.0f };
		for( unsigned int ray = 0; ray < SamplingCount; ray++ )
		{
			float sh_value = SphericalHarmonics<order>::SH( l, m, SampleArray.GetThetaArray()[ray], SampleArray.GetPhiArray()[ray] );
			coefficient[0] += RenderedColor[ray].r * sh_value;
			coefficient[1] += RenderedColor[ray].g * sh_value;
			coefficient[2] += RenderedColor[ray].b * sh_value;
		}
		coefficient[0] *= WeightFactor;
		coefficient[1] *= WeightFactor;
		coefficient[2] *= WeightFactor;

		outSHRed->coefficients[sh_i] = coefficient[0];
		outSHGreen->coefficients[sh_i] = coefficient[1];
		outSHBlue->coefficients[sh_i] = coefficient[2];

		if( ++m > l )
		{
			l++;
			m = -l;
		}
	}
}

template<unsigned int order>
void SHEngine::CalcAmbientOcclusionSH( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<order> *outSH )
{
	
	assert( outSH != NULL && outSH->GetOrder() < 6 );

	const unsigned int &RayCount = SamplingCount;
	float *SampledDistanceArray = new float[RayCount];

	SphericalSampleArray SampleArray( SamplingCount, SphericalSampleArray::SA_RAY_ARRAY|SphericalSampleArray::SA_THETA_ARRAY|SphericalSampleArray::SA_PHI_ARRAY );
	GenerateSphericalSampleArray( Position, &SampleArray );
	assert( SampleArray.GetRayArray() != NULL );
	assert( SampleArray.GetThetaArray() != NULL );
	assert( SampleArray.GetPhiArray() != NULL );

	assert( SampleArray.GetWeightArray() );
	for( unsigned int i = 0; i < SamplingCount; i++ )
	{
		float dot = Normal.DotProduct( GIVector3( SampleArray.GetRayArray()[i].dir ) );
		SampleArray.GetWeightArray()[i] = dot;
		//SampleArray.GetWeightArray()[i] = dot>0.0f?dot:0.0f;
	}

	SampleDistance( ThreadCount, Scene, RayCount, SampleArray.GetRayArray(), SampledDistanceArray );

	const float WeightFactor = 4.0f * FLOAT_PI / float(SamplingCount);

	int l = 0;
	int m = 0;
	for( unsigned int sh_i = 0; sh_i < outSH->GetNumberOfCoefficients(); sh_i++ )
	{
		float coefficient = 0.0f;
		for( unsigned int ray = 0; ray < SamplingCount; ray++ )
		{
			float sh_value = SphericalHarmonics<order>::SH( l, m, SampleArray.GetThetaArray()[ray], SampleArray.GetPhiArray()[ray] );
			float visibility = (SampledDistanceArray[ray]>=0.0f?0.0f:1.0f);
			float weight = SampleArray.GetWeightArray()[ray];
			if( weight < 0.0f )
				weight = 0.0f;
			coefficient += sh_value * visibility * weight;
			//coefficient += sh_value * visibility;
		}
		coefficient *= WeightFactor * 0.5f;

		// Ambient Occlusion
		//for( unsigned int ray = 0; ray < SamplingCount; ray++ )
		//{
		//	//float sh_value = SphericalHarmonics<order>::SH( l, m, SampleArray.GetThetaArray()[ray], SampleArray.GetPhiArray()[ray] );
		//	float visibility = (SampledDistanceArray[ray]>=0.0f?0.0f:1.0f);
		//	float weight = SampleArray.GetWeightArray()[ray];
		//	coefficient += visibility * weight;
		//}
		//coefficient *= WeightFactor/FLOAT_PI;

		outSH->coefficients[sh_i] = coefficient;

		if( ++m > l )
		{
			l++;
			m = -l;
		}
	}
}

template<unsigned int order>
void SHEngine::CalcDiffuseShadowedSH( unsigned int SamplingCount, unsigned int ThreadCount, 
						   GIScene *Scene, 
						   SphericalHarmonics<order> *outSHArray /*= NULL*/ )
{
	
	assert( outSHArray->GetOrder() < SH_MAX_ORDER );
	assert( Scene->GetVertexCount() > 0 );

	if( Scene->GetVertexCount() == 0 )
		return;

	if( Scene->GetSHList<order>() == NULL )
		Scene->InitializeTriangleSH( order );

	if( outSHArray == NULL )
		outSHArray = Scene->GetSHList<order>();

	assert( outSHArray != NULL );

	const unsigned int &RayCount = SamplingCount;
	float *SampledDistanceArray = new float[RayCount];

	SphericalSampleArray SampleArray( SamplingCount, SphericalSampleArray::SA_RAY_ARRAY|SphericalSampleArray::SA_THETA_ARRAY|SphericalSampleArray::SA_PHI_ARRAY|SphericalSampleArray::SA_WEIGHT_ARRAY );
	assert( SampleArray.GetRayArray() != NULL );
	assert( SampleArray.GetThetaArray() != NULL );
	assert( SampleArray.GetPhiArray() != NULL );
	assert( SampleArray.GetWeightArray() != NULL );

	float WeightFactor = 4.0f * FLOAT_PI / float(SamplingCount);
	WeightFactor /= FLOAT_PI;

	// TODO: cache 생각해서 최적화 해야함
	for( unsigned int vertex_i = 0; vertex_i < Scene->GetVertexCount(); vertex_i++ )
	{
		GIVector3 normal = Scene->GetVertexNormalList()[vertex_i];
		GIVector3 origin = Scene->GetVertexPositionList()[vertex_i] + normal * GetRayShootEpsilon();
		//GenerateHemisphericalSampleArray( origin, normal, &SampleArray );
		GenerateSphericalSampleArray( origin, normal, &SampleArray );

		SampleDistance( ThreadCount, Scene, RayCount, SampleArray.GetRayArray(), SampledDistanceArray );

		int l = 0;
		int m = 0;
		for( unsigned int sh_i = 0; sh_i < outSHArray->GetNumberOfCoefficients(); sh_i++ )
		{
			float coefficient = 0.0f;
			for( unsigned int ray = 0; ray < SamplingCount; ray++ )
			{
				float weight = SampleArray.GetWeightArray()[ray];
				if( weight <= 0.0f )
					continue;

				float sh_value = SphericalHarmonics<order>::SH( l, m, SampleArray.GetThetaArray()[ray], SampleArray.GetPhiArray()[ray] );
				float visibility = (SampledDistanceArray[ray]>=0.0f?0.0f:1.0f);
				coefficient += sh_value * visibility * weight;

				//coefficient += sh_value * visibility;
			}
			coefficient *= WeightFactor;
			outSHArray[vertex_i].coefficients[sh_i] = coefficient;

			if( ++m > l )
			{
				l++;
				m = -l;
			}
		}
	}
}

template<unsigned int order>
void SHEngine::CalcDiffuseInterrefelctionSH( unsigned int SamplingCount, unsigned int ThreadCount, unsigned int PathCount, GIScene *Scene, 
											 float ReductionFactor/* = 1.0f*/, 
											 SphericalHarmonicsRGB<order> *outSH_RGBArray /*= NULL*/ )
{
	
	assert( Scene->GetVertexCount() > 0 );
	assert( PathCount > 0 );

	if( Scene->GetSHList<order>() == NULL )
	{
		Scene->InitializeTriangleSH( order );
		CalcDiffuseShadowedSH<order>( SamplingCount, ThreadCount, Scene/*, Scene->GetSHList()*/ );
	}
	if( Scene->GetSH_RGBList<order>() == NULL )
		Scene->InitializeTriangleSH_RGB( order );

	SphericalHarmonics<order> *SHArray = Scene->GetSHList<order>();
	if( outSH_RGBArray == NULL )
		outSH_RGBArray = Scene->GetSH_RGBList<order>();

	assert( outSH_RGBArray != NULL );

	const unsigned int &RayCount = SamplingCount;

	SphericalSampleArray SampleArray( SamplingCount, SphericalSampleArray::SA_RAY_ARRAY|SphericalSampleArray::SA_WEIGHT_ARRAY );
	assert( SampleArray.GetRayArray() != NULL );
	assert( SampleArray.GetWeightArray() != NULL );

	GIHit *HitArray = new GIHit[RayCount];

	float WeightFactor = 2.0f * FLOAT_PI / float(SamplingCount);
	//WeightFactor /= FLOAT_PI;

	unsigned int COEFFICIENT_COUNT = SphericalHarmonicsRGB<order>::GetNumberOfCoefficients();

	SphericalHarmonicsRGB<order> *DI_Src = new SphericalHarmonicsRGB<order>[Scene->GetVertexCount()];
	SphericalHarmonicsRGB<order> *DI_Dest = new SphericalHarmonicsRGB<order>[Scene->GetVertexCount()];

	// DS값을 DI_Src에 복사
	for( unsigned int vertex_i = 0; vertex_i < Scene->GetVertexCount(); vertex_i++ )
	{
		outSH_RGBArray[vertex_i].Reset();
		DI_Src[vertex_i].r = SHArray[vertex_i];
		DI_Src[vertex_i].g = SHArray[vertex_i];
		DI_Src[vertex_i].b = SHArray[vertex_i];
	}

	// interreflection pass
	for( unsigned int pass = 0; pass < PathCount; pass++ )
	{
		// TODO: cache 생각해서 최적화 해야함
		for( unsigned int vertex_i = 0; vertex_i < Scene->GetVertexCount(); vertex_i++ )
		{
			GIVector3 normal = Scene->GetVertexNormalList()[vertex_i];
			GIVector3 origin = Scene->GetVertexPositionList()[vertex_i] + normal * GetRayShootEpsilon();

			GenerateHemisphericalSampleArray( origin, normal, &SampleArray );

			SampleHit( ThreadCount, Scene, RayCount, SampleArray.GetRayArray(), HitArray );

			//DI_Dest[vertex_i].Reset();

			SphericalHarmonicsRGB<order> CurSH;
			CurSH.Reset();

			for( unsigned int ray = 0; ray < SamplingCount; ray++ )
			{
				float cosWeight = SampleArray.GetWeightArray()[ray];
				if( cosWeight <= 0.0f )
					continue;

				// V == 0 ( 1-V == 1 )
				if( HitArray[ray].isHit() && HitArray[ray].backHit == false )
				{
					assert( HitArray[ray].dist > 0.001f );
					if( HitArray[ray].dist <= 0.001f )
						continue;
					GIVector4 albedo = Scene->GetAlbedo( HitArray[ray].triNum, HitArray[ray].u, HitArray[ray].v );
					const GITriangle hitTriangle = Scene->GetTriangle( HitArray[ray].triNum );

					const unsigned int v0 = hitTriangle.vg0.VertexNum;
					const unsigned int v1 = hitTriangle.vg1.VertexNum;
					const unsigned int v2 = hitTriangle.vg2.VertexNum;

					// TODO: 안으로 넣고 싶음
					SphericalHarmonicsRGB<order> interpolated_sh = 
						  DI_Src[v0] * (1.0f - HitArray[ray].u - HitArray[ray].v )
						+ DI_Src[v1] * HitArray[ray].u
						+ DI_Src[v2] * HitArray[ray].v;

					//SphericalHarmonics<order> sh = Scene->GetSH<order>( HitArray[ray].triNum, HitArray[ray].u, HitArray[ray].v );

					CurSH.r += interpolated_sh.r * (albedo.x * cosWeight);
					CurSH.g += interpolated_sh.g * (albedo.y * cosWeight);
					CurSH.b += interpolated_sh.b * (albedo.z * cosWeight);
				}
			}

			DI_Dest[vertex_i] = CurSH * WeightFactor * ReductionFactor;

			outSH_RGBArray[vertex_i] += DI_Dest[vertex_i];
		}
		SphericalHarmonicsRGB<order> *DI_Temp = DI_Dest;
		DI_Dest = DI_Src;
		DI_Src = DI_Temp;
	}

	//// DS값을 DI_Src에 복사
	//for( unsigned int vertex_i = 0; vertex_i < Scene->GetVertexCount(); vertex_i++ )
	//	for( unsigned int sh_i = 0; sh_i < outSH_RGBArray->GetNumberOfCoefficients(); sh_i++ )
	//{
	//	Scene->GetSH_RGBList<order>()[vertex_i].r = DI_Dest[vertex_i].r;
	//	Scene->GetSH_RGBList<order>()[vertex_i].g = DI_Dest[vertex_i].g;
	//	Scene->GetSH_RGBList<order>()[vertex_i].b = DI_Dest[vertex_i].b;
	//}


	SafeDeleteArray( &HitArray );
	SafeDeleteArray( &DI_Src );
	SafeDeleteArray( &DI_Dest );
}