/*!
 * \file		IrradianceCalcThread.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:16
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include <windows.h>

namespace GIEngine {

class GIScene;
class SceneAccelStructure;

class IrradianceCalcThread
{
public:
	static GIScene *mGIScene;
	static SceneAccelStructure *mAccelStructure;

	float *outPixel;

	GIRay *RayArray;
	unsigned int RayCount;

	float *CosWeightArray;

	static DWORD WINAPI IrradianceCalcThreadFunction( LPVOID lpParam );
};
};