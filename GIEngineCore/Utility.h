#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

namespace GIEngine
{
	inline float NormalizedRandom()
	{
		return float(rand())/float(RAND_MAX);
	}

	template<bool left_closed, bool right_closed>
	inline float NormalizedRandom()
	{
		unsigned int range = RAND_MAX + (left_closed?0:1) + (right_closed?0:1);
		return float(rand() + (left_closed?0:1))/float(range);
	}
}