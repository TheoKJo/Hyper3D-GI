#pragma once

#include "GIEnginePCH.h"

/*!
 * Valve 의 방법을 따름
 * \date 2010.12.06
 * \ref Shading in Valve's Source Engine
 */
class AmbientCube
{
public:
	void SetPosX( GIVector3 Irradiance ) { mIrradiances[0] = Irradiance; }
	void SetNegX( GIVector3 Irradiance ) { mIrradiances[1] = Irradiance; }
	void SetPosY( GIVector3 Irradiance ) { mIrradiances[2] = Irradiance; }
	void SetNegY( GIVector3 Irradiance ) { mIrradiances[3] = Irradiance; }
	void SetPosZ( GIVector3 Irradiance ) { mIrradiances[4] = Irradiance; }
	void SetNegZ( GIVector3 Irradiance ) { mIrradiances[5] = Irradiance; }

	GIVector3 GetPosX() { return mIrradiances[0]; }
	GIVector3 GetNegX() { return mIrradiances[1]; }
	GIVector3 GetPosY() { return mIrradiances[2]; }
	GIVector3 GetNegY() { return mIrradiances[3]; }
	GIVector3 GetPosZ() { return mIrradiances[4]; }
	GIVector3 GetNegZ() { return mIrradiances[5]; }

	const GIVector3& GetIrradiance( int index ) const { return mIrradiances[index]; }
	GIVector3* GetIrradianceArray() { return &mIrradiances[0]; }

	void ResetToBlack()
	{
		mIrradiances[0] = GIVector3( 0.0f, 0.0f, 0.0f ); mIrradiances[1] = GIVector3( 0.0f, 0.0f, 0.0f ); mIrradiances[2] = GIVector3( 0.0f, 0.0f, 0.0f );
		mIrradiances[3] = GIVector3( 0.0f, 0.0f, 0.0f ); mIrradiances[4] = GIVector3( 0.0f, 0.0f, 0.0f ); mIrradiances[5] = GIVector3( 0.0f, 0.0f, 0.0f );
	}

private:
	GIVector3 mIrradiances[6];
};