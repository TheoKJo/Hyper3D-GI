/*!
 * \file		Material.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:18
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#pragma warning( disable : 4996 )
#pragma warning( disable : 4100 ) //!< 이건 나중에 삭제~

#include <assert.h>
#include <string.h>

namespace GIEngine {
class GITexture
{
public:
	enum ETextureFormat 
	{
		ETF_GRAY, 
		ETF_RGB, 
		ETF_RGBA
	};

	GITexture() : m_Width(0), m_Height(0), mTextureFormat(ETF_RGBA), m_Data(NULL)
	{
	}

	void Initialize( unsigned int Width, unsigned int Height, ETextureFormat TextureFormat )
	{
		m_Width = Width;
		m_Height = Height;
		mTextureFormat = TextureFormat;
		unsigned int PixelSize = PixelByte( mTextureFormat );
		m_Data = new unsigned char[Width*Height*PixelSize];
	}

	unsigned char* GetData() { return m_Data; }

	// SAMPLE TYPE?
	// NEAREST, 

	// TODO: bClamp : false if repeat( wrap )
	GIVector4 GetColor( const GIVector2 &Texcoords, bool bClamp )
	{
		float s = Texcoords.u;
		float t = Texcoords.v;

		//GetTextureIndex( ix, iy, Texcoords, bClamp );

		if( bClamp )
		{
			if( s < 0.0f )
				s = 0.0f;
			if( t < 0.0f )
				t = 0.0f;
			if( 1.0f < s )
				s = 1.0f;
			if( 1.0f < t )
				t = 1.0f;
		}
		else
		{
			while( s < 0.0f )
				s += 1.0f;
			while( 1.0f < s )
				s -= 1.0f;

			while( t < 0.0f )
				t += 1.0f;
			while( 1.0f < t )
				t -= 1.0f;
		}

		assert( 0.0f <= s && s <= 1.0f && 0.0f <= t && t <= 1.0f );

		// nearest
		unsigned int uix = unsigned int(s * float(m_Width-1) + 0.5f);
		unsigned int uiy = unsigned int(t * float(m_Height-1) + 0.5f);

		assert( 0.0f <= uix && uix < m_Width && 0.0f <= uiy && uiy < m_Height );
	
		switch( mTextureFormat )
		{
		case ETF_GRAY:
			return GIVector4( m_Data[(uix + m_Width * uiy)], m_Data[(uix + m_Width * uiy)], m_Data[(uix + m_Width * uiy)] );
		case ETF_RGB:
			return GIVector4( m_Data[(uix + m_Width * uiy)*3+0], m_Data[(uix + m_Width * uiy)*3+1], m_Data[(uix + m_Width * uiy)*3+2], 1.0f );
		case ETF_RGBA:
			return GIVector4( m_Data[(uix + m_Width * uiy)*3+0], m_Data[(uix + m_Width * uiy)*3+1], m_Data[(uix + m_Width * uiy)*3+2], m_Data[(uix + m_Width * uiy)*3+3] );
		}
		assert( false );
		return GIVector4( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	unsigned int GetWidth() { return m_Width; }
	unsigned int GetHeight() { return m_Height; }

	void GetTextureIndex( int &x, int &y, const GIVector2 &Texcoords, bool bClamp )
	{
		
	}

	ETextureFormat GetTextureFormat() const { return mTextureFormat; }

private:
	unsigned int m_Width;
	unsigned int m_Height;
	ETextureFormat mTextureFormat;

	static unsigned int PixelByte( ETextureFormat TextureFormat )
	{
		switch( TextureFormat )
		{
		case ETF_GRAY:
			return 1;
		case ETF_RGB:
			return 3;
		case ETF_RGBA:
			return 4;
		}
		return 0;
	}

	unsigned char *m_Data;
};

class GIMaterialSet
{
public:
	GIVector4 AmbientColor;
	GIVector4 DiffuseColor;
	GIVector4 SpecularColor;
	float Shininess;
};

class GIMaterial
{
public:
	GIMaterial() : m_AlbedoTexture(NULL), m_MaterialName(NULL), m_TexturingType(ETT_COLOR) {}

	void SetMaterialName( const char *MaterialName )
	{
		SafeDeleteArray( &m_MaterialName );
		if( MaterialName == NULL )
			return;
		m_MaterialName = new char[strlen(MaterialName)+1];
		strcpy( m_MaterialName, MaterialName );
	}
	char* GetMaterialName() { return m_MaterialName; }

	void SetAlbedoTexture( GITexture *pTexture ) { m_AlbedoTexture = pTexture; }
	GITexture* GetAlbedoTexture() { return m_AlbedoTexture; }

	void SetAmbientColor( GIVector4 AmbientColor ) { m_MaterialSet.AmbientColor = AmbientColor; }
	void SetDiffuseColor( GIVector4 DiffuseColor ) { m_MaterialSet.DiffuseColor = DiffuseColor; }
	void SetSpecularColor( GIVector4 SpecularColor ) { m_MaterialSet.SpecularColor = SpecularColor; }
	void SetShininess( float Shininess ) { m_MaterialSet.Shininess = Shininess; }

	GIVector4 GetAmbientColor() { return m_MaterialSet.AmbientColor; }
	GIVector4 GetDiffuseColor() { return m_MaterialSet.DiffuseColor; }
	GIVector4 GetSpecularColor() { return m_MaterialSet.SpecularColor; }
	float GetShininess() { return m_MaterialSet.Shininess; }

	
	enum ETexturingType {
		ETT_REPLACE, ETT_MODULATE, ETT_COLOR, ETT_VERTEX_COLOR
	};

	ETexturingType m_TexturingType;

private:
	GITexture *m_AlbedoTexture;
	char *m_MaterialName;
	GIMaterialSet m_MaterialSet;
};
};