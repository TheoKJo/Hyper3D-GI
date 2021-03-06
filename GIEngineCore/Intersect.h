/*!
 * \file		Intersect.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:17
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "Structures.h"

// source code from Ingo Wald's PhD Thesis, Realtime Ray Tracing and Interactive Global Illumination (2004)

// lookup table for the modulo operation
//ALIGN(ALIGN_CACHELINE)
static const int modulo[] = {0,1,2,0,1};

inline bool RtIntersect( const GITriAccel &acc,const GIRay &ray, GIHit &outHit, float nearDist = GI_FLOAT_EPSILON, float farDist = GI_FLOAT_INFINITY )
{
	outHit.hit = false;

	const int ku = modulo[acc.k+1];
	const int kv = modulo[acc.k+2];
	// don' t prefetch here, assume data has already been prefetched

	// start high-latency division as early as possible
	const float nd = 1.0f/( ray.dir[acc.k] + 
		acc.n_u * ray.dir[ku] + acc.n_v * ray.dir[kv] );
	const float f = (acc.n_d - ray.org[acc.k] - acc.n_u * ray.org[ku] - acc.n_v * ray.org[kv]) * nd;

	// check for valid distance.
	if( !( nearDist <= f && f < GI_FLOAT_INFINITY ) )
		return false;

	// compute hitpoint positions on uv plane
	const float hu = (ray.org[ku] + f * ray.dir[ku]);
	const float hv = (ray.org[kv] + f * ray.dir[kv]);

	// check first barycentric coordinate
	const float lambda = (hu * acc.b_nu + hv * acc.b_nv + acc.b_d);

	if (lambda < 0.0f)
		return false;

	// check second barycentric coordinate
	const float mue = (hu * acc.c_nu + hv * acc.c_nv + acc.c_d);
	if( mue < 0.0f )
		return false;

	// check third barycentric coordinate
	if( lambda+mue > 1.0f )
		return false;

	// have a valid hitpoint here. store it.
	outHit.hit = true;
	outHit.dist = f;
	outHit.triNum = acc.triNum;
	outHit.u = lambda;
	outHit.v = mue;
	return true;
}