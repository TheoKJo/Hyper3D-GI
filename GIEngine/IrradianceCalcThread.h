/*!
 * \file		IrradianceCalcThread.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:16
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

class RtScene;

// TODO : PCH ·Î??
#include <windows.h>

class IrradianceCalcThread
{
public:
	static RtScene *mRtScene;

	float *outPixel;

	GIRay *RayArray;
	unsigned int RayCount;

	float *CosWeightArray;

	static DWORD WINAPI IrradianceCalcThreadFunction( LPVOID lpParam );
};