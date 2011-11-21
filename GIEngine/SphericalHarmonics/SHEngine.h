#pragma once

#include "../GIEngine.h"

#include "../SampleArray.h"

#define SH_MAX_ORDER 6

extern const SHMatrix<3,3> IdentityMatrix33;
extern SHMatrix<3,3> RotationMatrix_X33_m90;
extern SHMatrix<3,3> RotationMatrix_X33_p90;

namespace GIEngine { namespace SHEngine
{
	template<unsigned int order>
	void GenerateSphericalHarmonicsSampleArray( const GIVector3 &Position, SphericalHarmonicsSampleArray<order> *inoutSHSampleArray );
	template<unsigned int order>
	void GenerateSphericalHarmonicsSampleArray( const GIVector3 &Position, const GIVector3 &Normal, SphericalHarmonicsSampleArray<order> *inoutSHSampleArray );

	template<unsigned int order>
	SphericalHarmonics<order>* CreateCircleLight( float radius );

	SHMatrix<3,3> MakeSHRotateMatrix33( float alpha, float beta, float gamma );

	template<unsigned int order>
	void CalcSH_RGB( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, 
		const GIVector3 &Position, 
		SphericalHarmonics<order> *outSHRed, SphericalHarmonics<order> *outSHGreen, SphericalHarmonics<order> *outSHBlue );

	template<unsigned int order>
	void CalcAmbientOcclusionSH( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, 
		const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, 
		SphericalHarmonics<order> *outSH );

	/*!
	 * outSHArray가 NULL이면, Scene->SHList에 저장
	 */
	template<unsigned int order>
	void CalcDiffuseShadowedSH( unsigned int SamplingCount, unsigned int ThreadCount, 
		GIScene *Scene,
		SphericalHarmonics<order> *outSHArray = NULL );

	template<unsigned int order>
	void CalcDiffuseInterrefelctionSH( unsigned int SamplingCount, unsigned int ThreadCount, 
		unsigned int PathCount, 
		GIScene *Scene, 
		float ReductionFactor = 1.0f, 
		SphericalHarmonicsRGB<order> *outSH_RGBArray = NULL );

	template SphericalHarmonics<1>* SHEngine::CreateCircleLight( float radius );
	template SphericalHarmonics<2>* SHEngine::CreateCircleLight( float radius );
	template SphericalHarmonics<3>* SHEngine::CreateCircleLight( float radius );
	template SphericalHarmonics<4>* SHEngine::CreateCircleLight( float radius );
	template SphericalHarmonics<5>* SHEngine::CreateCircleLight( float radius );
	template SphericalHarmonics<6>* SHEngine::CreateCircleLight( float radius );

	template void CalcSH_RGB<1>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, SphericalHarmonics<1> *outSHRed, SphericalHarmonics<1> *outSHGreen, SphericalHarmonics<1> *outSHBlue );
	template void CalcSH_RGB<2>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, SphericalHarmonics<2> *outSHRed, SphericalHarmonics<2> *outSHGreen, SphericalHarmonics<2> *outSHBlue );
	template void CalcSH_RGB<3>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, SphericalHarmonics<3> *outSHRed, SphericalHarmonics<3> *outSHGreen, SphericalHarmonics<3> *outSHBlue );
	template void CalcSH_RGB<4>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, SphericalHarmonics<4> *outSHRed, SphericalHarmonics<4> *outSHGreen, SphericalHarmonics<4> *outSHBlue );
	template void CalcSH_RGB<5>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, SphericalHarmonics<5> *outSHRed, SphericalHarmonics<5> *outSHGreen, SphericalHarmonics<5> *outSHBlue );
	template void CalcSH_RGB<6>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, SphericalHarmonics<6> *outSHRed, SphericalHarmonics<6> *outSHGreen, SphericalHarmonics<6> *outSHBlue );

	template void CalcAmbientOcclusionSH<1>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<1> *outSH );
	template void CalcAmbientOcclusionSH<2>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<2> *outSH );
	template void CalcAmbientOcclusionSH<3>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<3> *outSH );
	template void CalcAmbientOcclusionSH<4>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<4> *outSH );
	template void CalcAmbientOcclusionSH<5>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<5> *outSH );
	template void CalcAmbientOcclusionSH<6>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, const GIVector3 &Position, const GIVector3 &Normal, const float LimitDistance, SphericalHarmonics<6> *outSH );

	template void CalcDiffuseShadowedSH<1>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, SphericalHarmonics<1> *outSHArray );
	template void CalcDiffuseShadowedSH<2>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, SphericalHarmonics<2> *outSHArray );
	template void CalcDiffuseShadowedSH<3>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, SphericalHarmonics<3> *outSHArray );
	template void CalcDiffuseShadowedSH<4>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, SphericalHarmonics<4> *outSHArray );
	template void CalcDiffuseShadowedSH<5>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, SphericalHarmonics<5> *outSHArray );
	template void CalcDiffuseShadowedSH<6>( unsigned int SamplingCount, unsigned int ThreadCount, GIScene *Scene, SphericalHarmonics<6> *outSHArray );

	template void CalcDiffuseInterrefelctionSH<1>( unsigned int SamplingCount, unsigned int ThreadCount, unsigned int PassCount, GIScene *Scene, float ReductionFactor, SphericalHarmonicsRGB<1> *outSH_RGBArray );
	template void CalcDiffuseInterrefelctionSH<2>( unsigned int SamplingCount, unsigned int ThreadCount, unsigned int PassCount, GIScene *Scene, float ReductionFactor, SphericalHarmonicsRGB<2> *outSH_RGBArray );
	template void CalcDiffuseInterrefelctionSH<3>( unsigned int SamplingCount, unsigned int ThreadCount, unsigned int PassCount, GIScene *Scene, float ReductionFactor, SphericalHarmonicsRGB<3> *outSH_RGBArray );
	template void CalcDiffuseInterrefelctionSH<4>( unsigned int SamplingCount, unsigned int ThreadCount, unsigned int PassCount, GIScene *Scene, float ReductionFactor, SphericalHarmonicsRGB<4> *outSH_RGBArray );
	template void CalcDiffuseInterrefelctionSH<5>( unsigned int SamplingCount, unsigned int ThreadCount, unsigned int PassCount, GIScene *Scene, float ReductionFactor, SphericalHarmonicsRGB<5> *outSH_RGBArray );
	template void CalcDiffuseInterrefelctionSH<6>( unsigned int SamplingCount, unsigned int ThreadCount, unsigned int PassCount, GIScene *Scene, float ReductionFactor, SphericalHarmonicsRGB<6> *outSH_RGBArray );
}}