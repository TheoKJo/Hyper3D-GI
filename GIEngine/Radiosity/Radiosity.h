#pragma once

#include "RadiosityScene.h"

namespace GIEngine { namespace Radiosity {

	struct RadiosityOption
	{
		unsigned int NumberOfThread;
		unsigned int RaySamplingPerQuad;
		float RayEpsilon;
	};
	extern const RadiosityOption DefaultRadiosityOption;

	bool BuildFormFactors( RadiosityScene *pRadiosityScene, const RadiosityOption &Option = DefaultRadiosityOption );

};};