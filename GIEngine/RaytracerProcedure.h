#pragma once

#include <windows.h>


#include "Raytracer/Raytracer.h"

// TODO : 颇老 昏力

class RaytracerProcedure
{
public:
	RaytracerProcedure() : mShadingFunction(NULL) {}
	virtual ~RaytracerProcedure() {}

	static RtScene *mRtScene;
	static KDTreeStructure *mKDTree;

	GIRay *mRayArray;
	unsigned int mRayCount;

	///*!
	// * mShading == false老 版快绰
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
		KDTreeStructure *pKDTree = (KDTreeStructure*)mRtScene->GetAccStructure();
		for( unsigned int i = 0; i < RayCount; i++ )
		{
			GIVector4 RenderedColor = GetRaytracer()->BackgroundColor;
			GetRaytracer()->ShootRay( mRtScene, RayArray[i], &RenderedColor, pKDTree );
			RenderedColor.CopyToFloatArray( &outPixel[i*4] );
		}
		return 0;
	}*/

	static DWORD WINAPI ShadingProcedure( LPVOID lpParam )
	{
		RaytracerProcedure *pRaytracerProcedure = (RaytracerProcedure*)lpParam;
		GIVector3 *ColorData = (GIVector3*)pRaytracerProcedure->mOutData;

		for( unsigned int i = 0; i < pRaytracerProcedure->mRayCount; i++ )
		{
			GIVector4 RenderedColor;
			GIHit Hit = GIEngine::Raytracer::ShootRay( mRtScene, pRaytracerProcedure->mRayArray[i] );

			GIEngine::Raytracer::Shading( mRtScene, pRaytracerProcedure->mRayArray[i], Hit, 2, &RenderedColor );
			ColorData[i] = RenderedColor.ToVector3();
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
			HitData[i] = GIEngine::Raytracer::ShootRay( mRtScene, pRaytracerProcedure->mRayArray[i] );
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
			GIHit Hit = GIEngine::Raytracer::ShootRay( mRtScene, pRaytracerProcedure->mRayArray[i] );

			assert( pRaytracerProcedure->mShadingFunction != NULL );
			pRaytracerProcedure->mShadingFunction( mRtScene, pRaytracerProcedure->mRayArray[i], Hit, 2, &RenderedColor );
			ColorData[i] = RenderedColor.ToVector3();
		}
		return 0;
	}
};
