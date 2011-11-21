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