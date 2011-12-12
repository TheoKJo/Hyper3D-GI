/*!
 * \file		Radiosity.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

namespace GIEngine { namespace Radiosity {

	class RadiosityScene;

	struct RadiosityOption
	{
		RadiosityOption() : NumberOfThread(4), RaySamplingPerQuad(100), RayEpsilon(0.001f)
		{}

		unsigned int NumberOfThread;
		unsigned int RaySamplingPerQuad;
		float RayEpsilon;
	};
	extern const RadiosityOption DefaultRadiosityOption;

	bool BuildFormFactors( RadiosityScene *pRadiosityScene, const RadiosityOption &Option /*= DefaultRadiosityOption*/ );

};};