/*!
 * \file		RadiosityLightMapMaterial.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

namespace GIEngine { namespace Radiosity {

class RadiosityLightMapMaterial
{
public:
	RadiosityLightMapMaterial( unsigned TextureWidth, unsigned int TextureHeight )
		: m_Width(TextureWidth), m_Height(TextureHeight)
	{
		m_QuadrangleNumList = new int[m_Width*m_Height];
		// TODO: assert m_QuadrangleNumList != NULL

		for( unsigned int i = 0; i < m_Width*m_Height; i++ )
			m_QuadrangleNumList[i] = -1;
	}

	unsigned int GetWidth() { return m_Width; }
	unsigned int GetHeight() { return m_Height; }

private:
	int *m_QuadrangleNumList;
	unsigned int m_Width;
	unsigned int m_Height;
};

};};