#include "GIEnginePCH.h"

#include "SHEngine.h"
#include "../GIEngine.h"

using namespace GIEngine;

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

	SphericalSampleArray SampleArray( CREATING_LIGHT_SAMPLE_COUNT, true, true );
	assert( SampleArray.GetThetaArray() != NULL );
	assert( SampleArray.GetPhiArray() != NULL );

	float WeightFactor = 4.0f * FLOAT_PI / float(CREATING_LIGHT_SAMPLE_COUNT);

	GIEngine::GetRaytracer()->InitializeSphericalRays( RtVector3( 0.0f, 0.0f, 0.0f ), &SampleArray );

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
void SHEngine::CalcSH_RGB( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const RtVector3 &Position, 
						  SphericalHarmonics<order> *outSHRed, SphericalHarmonics<order> *outSHGreen, SphericalHarmonics<order> *outSHBlue )
{
	assert( GetRaytracer() != NULL );
	assert( outSHRed->GetOrder() == outSHBlue->GetOrder() && outSHBlue->GetOrder() == outSHGreen->GetOrder() );
	assert( outSHRed->GetOrder() < 6 );

	const unsigned int &RayCount = SamplingCount;
	RtRay *RayArray = new RtRay[RayCount];
	float *ThetaArray = new float[RayCount];
	float *PhiArray = new float[RayCount];
	RtVector3 *RenderedColor = new RtVector3[RayCount];
	GetRaytracer()->InitializeSphericalRays( SamplingCount, Position, RayArray, ThetaArray, PhiArray );

	// TODO: 샘플링 미리 계산

	// TODO: Coordinate 확인???

	// Depth 설정
	SampleColor( ThreadCount, Scene, RayArray, RayCount, RenderedColor );

	const float WeightFactor = 4.0f * FLOAT_PI / float(SamplingCount);

	int l = 0;
	int m = 0;
	for( unsigned int sh_i = 0; sh_i < outSHRed->GetNumberOfCoefficients(); sh_i++ )
	{
		float coefficient[3] = { 0.0f, 0.0f, 0.0f };
		for( unsigned int ray = 0; ray < SamplingCount; ray++ )
		{
			float sh_value = SphericalHarmonics<order>::SH( l, m, ThetaArray[ray], PhiArray[ray] );
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
void SHEngine::CalcAmbientOcclusionSH( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const RtVector3 &Position, const RtVector3 &Normal, const float LimitDistance, SphericalHarmonics<order> *outSH )
{
	assert( GetRaytracer() != NULL );
	assert( outSH != NULL && outSH->GetOrder() < 6 );

	const unsigned int &RayCount = SamplingCount;
	float *SampledDistanceArray = new float[RayCount];

	SphericalSampleArray SampleArray( SamplingCount, true, true );
	GetRaytracer()->InitializeSphericalRays( Position, &SampleArray );
	assert( SampleArray.GetThetaArray() != NULL );
	assert( SampleArray.GetPhiArray() != NULL );

	assert( SampleArray.GetWeightArray() );
	for( unsigned int i = 0; i < SamplingCount; i++ )
	{
		float dot = Normal.DotProduct( RtVector3( SampleArray.GetRayArray()[i].dir ) );
		SampleArray.GetWeightArray()[i] = dot;
		//SampleArray.GetWeightArray()[i] = dot>0.0f?dot:0.0f;
	}

	SampleDistance( ThreadCount, Scene, SampleArray.GetRayArray(), RayCount, SampledDistanceArray );

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
	assert( GetRaytracer() != NULL );
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

	SphericalSampleArray SampleArray( SamplingCount, true, true );
	assert( SampleArray.GetThetaArray() != NULL );
	assert( SampleArray.GetPhiArray() != NULL );

	float WeightFactor = 4.0f * FLOAT_PI / float(SamplingCount);
	WeightFactor /= FLOAT_PI;

	// TODO: cache 생각해서 최적화 해야함
	for( unsigned int vertex_i = 0; vertex_i < Scene->GetVertexCount(); vertex_i++ )
	{
		RtVector3 normal = Scene->GetVertexNormalList()[vertex_i];
		RtVector3 origin = Scene->GetVertexPositionList()[vertex_i] + normal * GetRayShootEpsilon();
		//GetRaytracer()->InitializeHemisphericalRays( origin, normal, &SampleArray );
		GetRaytracer()->InitializeSphericalRays( origin, normal, &SampleArray );

		SampleDistance( ThreadCount, Scene, SampleArray.GetRayArray(), RayCount, SampledDistanceArray );

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
									 SphericalHarmonicsRGB<order> *outSH_RGBArray /*= NULL*/ )
{
	assert( GetRaytracer() != NULL );
	assert( Scene->GetVertexCount() > 0 );

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

	SphericalSampleArray SampleArray( SamplingCount, true, true );
	assert( SampleArray.GetThetaArray() != NULL );
	assert( SampleArray.GetPhiArray() != NULL );

	RtHit *HitArray = new RtHit[RayCount];

	float WeightFactor = 2.0f * FLOAT_PI / float(SamplingCount);
	WeightFactor /= FLOAT_PI;

	unsigned int COEFFICIENT_COUNT = SphericalHarmonicsRGB<order>::GetNumberOfCoefficients();
	float *coefficients = new float[COEFFICIENT_COUNT * 3];

	SphericalHarmonicsRGB<order> *SecondPass = new SphericalHarmonicsRGB<order>[Scene->GetVertexCount()];

	// interreflection pass
	//for( unsigned int pass = 0; pass < PathCount; pass++ )
	{
		// TODO: cache 생각해서 최적화 해야함
		for( unsigned int vertex_i = 0; vertex_i < Scene->GetVertexCount(); vertex_i++ )
		{
			RtVector3 normal = Scene->GetVertexNormalList()[vertex_i];
			RtVector3 origin = Scene->GetVertexPositionList()[vertex_i] + normal * GetRayShootEpsilon();
			const SphericalHarmonics<order> &SH_DS = SHArray[vertex_i];

			GetRaytracer()->InitializeHemisphericalRays( origin, normal, &SampleArray );

			/*RtVector3 origin = Scene->GetVertexPositionList()[vertex_i] + Scene->GetVertexNormalList()[vertex_i] * RAY_SHOOT_EPSILON;
			GetRaytracer()->InitializeSphericalRays( origin, &SampleArray );*/
			SampleHit( ThreadCount, Scene, SampleArray.GetRayArray(), RayCount, HitArray );

			outSH_RGBArray[vertex_i].Reset();
			
			memset( coefficients, 0, sizeof(float) * COEFFICIENT_COUNT * 3 );

			for( unsigned int ray = 0; ray < SamplingCount; ray++ )
			{
				// V == 0 ( 1-V == 1 )
				if( HitArray[ray].isHit() && HitArray[ray].backHit == false )
				{
					assert( HitArray[ray].dist > 0.001f );
					if( HitArray[ray].dist <= 0.001f )
						continue;
					RtVector4 albedo = Scene->GetAlbedo( HitArray[ray].triNum, HitArray[ray].u, HitArray[ray].v );
					SphericalHarmonics<order> sh = Scene->GetSH<order>( HitArray[ray].triNum, HitArray[ray].u, HitArray[ray].v );

					for( unsigned int sh_i = 0; sh_i < outSH_RGBArray->GetNumberOfCoefficients(); sh_i++ )
					{
						float cosWeight = SampleArray.GetWeightArray()[ray];
						if( cosWeight > 0.0f )
						{
							coefficients[sh_i*3+0] += sh.coefficients[sh_i] * albedo.x * cosWeight;
							coefficients[sh_i*3+1] += sh.coefficients[sh_i] * albedo.y * cosWeight;
							coefficients[sh_i*3+2] += sh.coefficients[sh_i] * albedo.z * cosWeight;
						}
					}
				}
			}

			for( unsigned int sh_i = 0; sh_i < outSH_RGBArray->GetNumberOfCoefficients(); sh_i++ )
			{
				// Shadowed Only
				/*outSH_RGBArray[vertex_i].r.coefficients[sh_i] = SH_DS.coefficients[sh_i];
				outSH_RGBArray[vertex_i].g.coefficients[sh_i] = SH_DS.coefficients[sh_i];
				outSH_RGBArray[vertex_i].b.coefficients[sh_i] = SH_DS.coefficients[sh_i];*/
				// Interreflection Only
				outSH_RGBArray[vertex_i].r.coefficients[sh_i] = coefficients[sh_i*3+0] * WeightFactor;
				outSH_RGBArray[vertex_i].g.coefficients[sh_i] = coefficients[sh_i*3+1] * WeightFactor;
				outSH_RGBArray[vertex_i].b.coefficients[sh_i] = coefficients[sh_i*3+2] * WeightFactor;
				// Both
				/*outSH_RGBArray[vertex_i].r.coefficients[sh_i] = SH_DS.coefficients[sh_i] + coefficients[sh_i*3+0] * WeightFactor / FLOAT_PI;
				outSH_RGBArray[vertex_i].g.coefficients[sh_i] = SH_DS.coefficients[sh_i] + coefficients[sh_i*3+1] * WeightFactor / FLOAT_PI;
				outSH_RGBArray[vertex_i].b.coefficients[sh_i] = SH_DS.coefficients[sh_i] + coefficients[sh_i*3+2] * WeightFactor / FLOAT_PI;*/
			}

			
		}
	}
	SafeDeleteArray( &coefficients );
	delete[] SecondPass;
}