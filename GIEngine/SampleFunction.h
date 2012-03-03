/*!
 * \file		SampleFunction.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:17
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#ifndef SAMPLE_FUNCTION_H
#define SAMPLE_FUNCTION_H

namespace GIEngine {

/*
 * theta: [0, PI]
 * phi: [0, 2PI]
 */
typedef float (*SampleFunction)( float theta, float phi );

typedef void (*SphericalCoordinateSamplingFunction)( float xi1, float xi2, float *outTheta, float *outPhi );

//! Uniform
//! Lambertian surface
inline void UniformSphericalCoordinateSamplingFunction( float xi1, float xi2, float *outTheta, float *outPhi )
{
	*outTheta = 2.0f * acos( sqrt( 1.0f - xi1 ) );
	*outPhi = 2.0f * GI_FLOAT_PI * xi2;
}

inline void UniformHemisphericalCoordinateSamplingFunction( float xi1, float xi2, float *outTheta, float *outPhi )
{
	*outTheta = acos( xi1 );
	*outPhi = 2.0f * GI_FLOAT_PI * xi2;
}

//inline void UniformSphericalCoordinateSamplingFunction( float xi1, float xi2, float *outTheta, float *outPhi )
//{
//	//NormalizedRandom<true,false>(
//	*outTheta = 2.0f * acos( sqrt( 1.0f - xi1 ) );
//	*outPhi = 2.0f * GI_FLOAT_PI * xi2;
//}

};

#endif