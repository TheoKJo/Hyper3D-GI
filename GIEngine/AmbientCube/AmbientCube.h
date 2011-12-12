/*!
 * \file		AmbientCube.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:12
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "../GIEnginePCH.h"

/*!
 * Valve 의 방법을 따름
 * \date 2010.12.06
 * \ref Shading in Valve's Source Engine
 */
class AmbientCube
{
public:
	void SetPosX( GIVector3 Irradiance ) { m_Irradiances[0] = Irradiance; }
	void SetNegX( GIVector3 Irradiance ) { m_Irradiances[1] = Irradiance; }
	void SetPosY( GIVector3 Irradiance ) { m_Irradiances[2] = Irradiance; }
	void SetNegY( GIVector3 Irradiance ) { m_Irradiances[3] = Irradiance; }
	void SetPosZ( GIVector3 Irradiance ) { m_Irradiances[4] = Irradiance; }
	void SetNegZ( GIVector3 Irradiance ) { m_Irradiances[5] = Irradiance; }

	GIVector3 GetPosX() { return m_Irradiances[0]; }
	GIVector3 GetNegX() { return m_Irradiances[1]; }
	GIVector3 GetPosY() { return m_Irradiances[2]; }
	GIVector3 GetNegY() { return m_Irradiances[3]; }
	GIVector3 GetPosZ() { return m_Irradiances[4]; }
	GIVector3 GetNegZ() { return m_Irradiances[5]; }

	const GIVector3& GetIrradiance( int index ) const { return m_Irradiances[index]; }
	GIVector3* GetIrradianceArray() { return &m_Irradiances[0]; }

	void ResetToBlack()
	{
		m_Irradiances[0] = GIVector3( 0.0f, 0.0f, 0.0f ); m_Irradiances[1] = GIVector3( 0.0f, 0.0f, 0.0f ); m_Irradiances[2] = GIVector3( 0.0f, 0.0f, 0.0f );
		m_Irradiances[3] = GIVector3( 0.0f, 0.0f, 0.0f ); m_Irradiances[4] = GIVector3( 0.0f, 0.0f, 0.0f ); m_Irradiances[5] = GIVector3( 0.0f, 0.0f, 0.0f );
	}

private:
	GIVector3 m_Irradiances[6];
};