#include "GIEnginePCH.h"

#include "IrradianceCalcThread.h"

#include "Raytracer/Raytracer.h"

RtScene* IrradianceCalcThread::mRtScene = NULL;

using namespace GIEngine;

DWORD WINAPI IrradianceCalcThread::IrradianceCalcThreadFunction( LPVOID lpParam )
{
	IrradianceCalcThread *pThreadData = (IrradianceCalcThread*)lpParam;
	KDTreeStructure *pKDTree = (KDTreeStructure*)mRtScene->GetAccStructure();

	Raytracer::RaytracerOption Option;
	Option.RayEpsilon = mRtScene->GetSceneEpsilon();
	Option.PassThroughBackHit = true;
	Option.PassThroughSelfHit = true;

	for( unsigned int i = 0; i < pThreadData->RayCount; i++ )
	{
		GIVector4 RenderedColor;
		GIHit Hit = Raytracer::ShootRay( pThreadData->mRtScene, pThreadData->RayArray[i] );
		Raytracer::Shading( pThreadData->mRtScene, pThreadData->RayArray[i], Hit, 2, &RenderedColor );

		pThreadData->outPixel[0] += pThreadData->CosWeightArray[i] * RenderedColor.x;
		pThreadData->outPixel[1] += pThreadData->CosWeightArray[i] * RenderedColor.y;
		pThreadData->outPixel[2] += pThreadData->CosWeightArray[i] * RenderedColor.z;
	}
	return 0;
}