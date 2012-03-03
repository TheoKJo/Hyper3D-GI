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

namespace GIEngine {
class GICamera
{
public:
	GICamera();
	virtual ~GICamera();

	GIVector3 GetOrigin() { return Origin; }
	GIVector3 GetDirection() { return Direction; }
	GIVector3 GetRightDirection() { return Direction.CrossProduct( UpDirection ); }
	GIVector3 GetUpDirection() { return UpDirection; }

	float GetFovy() { return Fovy; }
	// TODO: º¯¼ö·Î
	float GetAspectRatio() { return (float)Width/(float)Height; }

	float GetNearDistance() { return NearDistance; }
	float GetFarDistance() { return FarDistance; }

// const version
	const GIVector3& GetOrigin() const { return Origin; }
	const GIVector3& GetDirection() const { return Direction; }
	GIVector3 GetRightDirection() const { return Direction.CrossProduct( UpDirection ); }
	const GIVector3& GetUpDirection() const { return UpDirection; }

	// TODO: get/set, private
//private:
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

class GIRayCamera : public GICamera
{
public:
	GIRayCamera();
	virtual ~GIRayCamera();

	void CreateRays();
	unsigned int GetCameraRayCount();
	GIRay* GetCameraRays();

private:
	unsigned int m_RayCount;
	GIRay *m_RayArray;
	bool m_bRaysDirty;
};
};
