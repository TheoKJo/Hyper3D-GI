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
