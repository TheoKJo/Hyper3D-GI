/*!
 * \file		RaytracerProcedure.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:13
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include <windows.h>


#include "Raytracer.h"
//#include <Raytracer/Raytracer.h>

// TODO : 파일 삭제

using namespace GIEngine;

class RaytracerProcedure
{
public:
	RaytracerProcedure() : mShadingFunction(NULL) {}
	virtual ~RaytracerProcedure() {}

	static GIScene *mGIScene;
	static KDTreeStructure *mKDTree;

	GIRay *mRayArray;
	unsigned int mRayCount;

	///*!
	// * mShading == false일 경우는
	// * x = distance
	// * y >= 0, hit
	// * y < 0, not hit
	// */
	//GIVector3 *mOutColor;
	void *mOutData;
	//void 
	ShadingFunction mShadingFunction;

	/*unsigned int ThreadProcedure( void *pData )
	{
		KDTreeStructure *pKDTree = (KDTreeStructure*)mGIScene->GetAccStructure();
		for( unsigned int i = 0; i < RayCount; i++ )
		{
			GIVector4 RenderedColor = GetRaytracer()->BackgroundColor;
			GetRaytracer()->ShootRay( mGIScene, RayArray[i], &RenderedColor, pKDTree );
			RenderedColor.CopyToFloatArray( &outPixel[i*4] );
		}
		return 0;
	}*/

	static DWORD WINAPI ShootAndShadingProcedure( LPVOID lpParam )
	{
		RaytracerProcedure *pRaytracerProcedure = (RaytracerProcedure*)lpParam;
		GIVector3 *ColorData = (GIVector3*)pRaytracerProcedure->mOutData;

		for( unsigned int i = 0; i < pRaytracerProcedure->mRayCount; i++ )
		{
			GIVector4 RenderedColor;
			GIHit Hit = GIEngine::Raytracer::ShootRay( mGIScene, mKDTree, pRaytracerProcedure->mRayArray[i] );

			GIEngine::Raytracer::Shading( mGIScene, mKDTree, pRaytracerProcedure->mRayArray[i], Hit, 2, &RenderedColor );
			ColorData[i] = RenderedColor.xyz();
		}
		return 0;
	}

	static DWORD WINAPI HitProcedure( LPVOID lpParam )
	{
		RaytracerProcedure *pRaytracerProcedure = (RaytracerProcedure*)lpParam;
		GIHit *HitData = (GIHit*)pRaytracerProcedure->mOutData;

		for( unsigned int i = 0; i < pRaytracerProcedure->mRayCount; i++ )
		{
			GIVector4 RenderedColor;
			HitData[i] = GIEngine::Raytracer::ShootRay( mGIScene, mKDTree, pRaytracerProcedure->mRayArray[i] );
		}
		return 0;
	}

	static DWORD WINAPI UserDefinedShadingProcedure( LPVOID lpParam )
	{
		RaytracerProcedure *pRaytracerProcedure = (RaytracerProcedure*)lpParam;
		GIVector3 *ColorData = (GIVector3*)pRaytracerProcedure->mOutData;

		for( unsigned int i = 0; i < pRaytracerProcedure->mRayCount; i++ )
		{
			GIVector4 RenderedColor;
			GIHit Hit = GIEngine::Raytracer::ShootRay( mGIScene, mKDTree, pRaytracerProcedure->mRayArray[i] );

			assert( pRaytracerProcedure->mShadingFunction != NULL );
			pRaytracerProcedure->mShadingFunction( mGIScene, mKDTree, pRaytracerProcedure->mRayArray[i], Hit, 2, &RenderedColor );
			ColorData[i] = RenderedColor.xyz();
		}
		return 0;
	}
};
