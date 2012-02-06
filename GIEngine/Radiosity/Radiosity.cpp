/*!
 * \file		Radiosity.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include "../GIEngine.h"
#include "../SampleArray.h"
#include "Radiosity.h"

#include "RadiosityScene.h"

using namespace GIEngine;

//const Radiosity::RadiosityOption Radiosity::DefaultRadiosityOption = { 4, 1000, 0.0001f };

bool Radiosity::BuildFormFactors( RadiosityScene *pRadiosityScene, const RadiosityOption &Option )
{
	if( !pRadiosityScene->IsFormFactorInitialize() && !pRadiosityScene->InitializeFormFactor() )
		return false;

	//assert( pRadiosityScene->IsFormFactorInitialize() );

	// Check Quadrangles initialized
	//pRadiosityScene->
	GIScene *pScene = pRadiosityScene->GetScene();
	
	unsigned int QuadrangleCount = pRadiosityScene->GetQuadrangleCount();
	GIQuadrangle *QuadrangleList = pRadiosityScene->GetQuadrangleList();

	unsigned int RayCount = Option.RaySamplingPerQuad;
	GIRay *RayArray = new GIRay[RayCount];
	GIHit *HitArray = new GIHit[RayCount];

	SphericalSampleArray *RaySampleArray = new SphericalSampleArray( RayCount, SphericalSampleArray::SA_RAY_ARRAY | SphericalSampleArray::SA_WEIGHT_ARRAY );

	unsigned int *FormFactorHitCount = new unsigned int [QuadrangleCount*QuadrangleCount];
	for( unsigned int i = 0; i < QuadrangleCount*QuadrangleCount; i++ )
		FormFactorHitCount[i] = 0;

	for( unsigned int SrcQuadNum = 0; SrcQuadNum < QuadrangleCount; SrcQuadNum++ )
	{
		const GIQuadrangle &Quadrangle = QuadrangleList[SrcQuadNum];

		// TODO: Backhit on
		GenerateHemisphericalSampleArray( Quadrangle.CenterPosition, Quadrangle.CenterNormal, RaySampleArray );
		//Option.NumberOfThread
		assert( false ); // NOT NULL!!!
		SampleHit( pScene, NULL, RaySampleArray->GetRayCount(), RaySampleArray->GetRayArray(), HitArray );

		for( unsigned int j = 0; j < RaySampleArray->GetRayCount(); j++ )
		{
			if( HitArray[j].isHit() && !HitArray[j].backHit )
			{
				// TODO: 실패시는??
				unsigned int DestQuadNum;
				if( pRadiosityScene->GetQuadrangle( &DestQuadNum, HitArray[j].triNum, HitArray[j].u, HitArray[j].v ) )
					FormFactorHitCount[SrcQuadNum + DestQuadNum * QuadrangleCount]++;				
			}
		}
	}

	for( unsigned int DestQuadNum = 0; DestQuadNum < QuadrangleCount; DestQuadNum++ )
		for( unsigned int SrcQuadNum = 0; SrcQuadNum < QuadrangleCount; SrcQuadNum++ )
	{
		if( SrcQuadNum == DestQuadNum )
			pRadiosityScene->SetFormFactor( SrcQuadNum, DestQuadNum, 0.0f );
		else if( SrcQuadNum < DestQuadNum )
		{
			float SrcArea = 1.0f, DestArea = 1.0f;
			float FormFactor1 = float(FormFactorHitCount[SrcQuadNum + DestQuadNum * QuadrangleCount]) / SrcArea;
			float FormFactor2 = float(FormFactorHitCount[DestQuadNum + SrcQuadNum * QuadrangleCount]) / DestArea;
			pRadiosityScene->SetFormFactor( SrcQuadNum, DestQuadNum, FormFactor1 * FormFactor2 * 0.5f );
		}
	}
		
	
	return true;
}