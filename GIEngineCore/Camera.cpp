/*!
 * \file		Camera.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:17
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEngineCorePCH.h"

#include "Camera.h"

using namespace GIEngine;


GICamera::GICamera()
{
}

GICamera::~GICamera()
{
}

GIRayCamera::GIRayCamera()
	: m_RayCount(0), m_RayArray(NULL) //m_bRaysDirty(false)
{
	
}

GIRayCamera::~GIRayCamera()
{
	SafeDeleteArray( &m_RayArray );
}

void GIRayCamera::CreateRays()
{
	// TODO: 잘못된 width, height 값(0 혹은 쓰레기값)에 대한 대비가 없음.
	assert( Width != 0 && Height != 0 );

	m_RayCount = Width * Height;
	if( m_RayArray == NULL )
		m_RayArray = new GIRay[m_RayCount];

	float halfHeight = tan( TO_RADIAN( GetFovy() ) );
	float halfWidth = halfHeight * GetAspectRatio();
	GIVector3 LeftTop = GetOrigin() + GetDirection() + GetUpDirection() * halfHeight + GetRightDirection() * -halfWidth;

	float fWidthPixel = float(Width);
	float fHeightPixel = float(Height);

	for( unsigned int y = 0; y < Height; y++ )
		for( unsigned int x = 0; x < Width; x++ )
		{
			GIVector3 dest = LeftTop + GetDirection() * float(y)/fWidthPixel+ GetRightDirection() * float(x)/fHeightPixel;

			GIRay &ray = m_RayArray[x+y*Width];
			GIVector3 direction = (dest - GetOrigin()).GetNormalized();
			ray.dir << direction;
			ray.org << GetOrigin();
			ray.NearDistance = GetNearDistance();
			ray.FarDistance = GetFarDistance();
		}

	m_bRaysDirty = false;
}

unsigned int GIRayCamera::GetCameraRayCount()
{
	return m_RayCount;
}

GIRay* GIRayCamera::GetCameraRays()
{
	return m_RayArray;
}