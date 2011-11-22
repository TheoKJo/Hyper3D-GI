/*!
 * \file		Camera.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:17
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "Global.h"

class GICamera
{
public:
	GICamera();
	virtual ~GICamera();

	GIVector3 Origin;
	GIVector3 Direction;
	GIVector3 UpDirection;

	float Fovy;
	unsigned int Width;
	unsigned int Height;

	// Default : [0, \inf)
	float NearDistance;
	float FarDistance;
};
