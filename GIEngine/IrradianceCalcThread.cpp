/*!
 * \file		IrradianceCalcThread.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:16
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include "IrradianceCalcThread.h"
#include "Raytracer/Raytracer.h"
#include "Raytracer/SceneAccelStructure.h"

#include <Scene.h>

using namespace GIEngine;

GIScene* IrradianceCalcThread::mGIScene = NULL;

DWORD WINAPI IrradianceCalcThread::IrradianceCalcThreadFunction( LPVOID lpParam )
{
	IrradianceCalcThread *pThreadData = (IrradianceCalcThread*)lpParam;

	/*RaytracerOption Option;
	Option.RayEpsilon = mGIScene->GetSceneEpsilon();
	Option.PassThroughBackHit = true;
	Option.PassThroughSelfHit = true;*/

	for( unsigned int i = 0; i < pThreadData->RayCount; i++ )
	{
		GIVector4 RenderedColor;
		GIHit Hit = Raytracer::ShootRay( pThreadData->mGIScene, mAccelStructure->GetKDTree(), pThreadData->RayArray[i] );
		Raytracer::Shading( pThreadData->mGIScene, mAccelStructure->GetKDTree(), pThreadData->RayArray[i], Hit, 2, &RenderedColor );

		pThreadData->outPixel[0] += pThreadData->CosWeightArray[i] * RenderedColor.x;
		pThreadData->outPixel[1] += pThreadData->CosWeightArray[i] * RenderedColor.y;
		pThreadData->outPixel[2] += pThreadData->CosWeightArray[i] * RenderedColor.z;
	}
	return 0;
}