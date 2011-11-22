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

class GITexture
{
public:
	enum ETextureFormat 
	{
		ETF_GRAY, 
		ETF_RGB, 
		ETF_RGBA
	};

	GITexture() : mWidth(0), mHeight(0), mTextureFormat(ETF_RGBA), mData(NULL)
	{
	}

	void Initialize( unsigned int Width, unsigned int Height, ETextureFormat TextureFormat )
	{
		mWidth = Width;
		mHeight = Height;
		mTextureFormat = TextureFormat;
		unsigned int PixelSize = PixelByte( mTextureFormat );
		mData = new unsigned char[Width*Height*PixelSize];
	}

	unsigned char* GetData() { return mData; }

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
		unsigned int uix = unsigned int(s * float(mWidth-1) + 0.5f);
		unsigned int uiy = unsigned int(t * float(mHeight-1) + 0.5f);

		assert( 0.0f <= uix && uix < mWidth && 0.0f <= uiy && uiy < mHeight );
	
		switch( mTextureFormat )
		{
		case ETF_GRAY:
			return GIVector4( mData[(uix + mWidth * uiy)], mData[(uix + mWidth * uiy)], mData[(uix + mWidth * uiy)] );
		case ETF_RGB:
			return GIVector4( mData[(uix + mWidth * uiy)*3+0], mData[(uix + mWidth * uiy)*3+1], mData[(uix + mWidth * uiy)*3+2], 1.0f );
		case ETF_RGBA:
			return GIVector4( mData[(uix + mWidth * uiy)*3+0], mData[(uix + mWidth * uiy)*3+1], mData[(uix + mWidth * uiy)*3+2], mData[(uix + mWidth * uiy)*3+3] );
		}
		assert( false );
		return GIVector4( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	unsigned int GetWidth() { return mWidth; }
	unsigned int GetHeight() { return mHeight; }

	void GetTextureIndex( int &x, int &y, const GIVector2 &Texcoords, bool bClamp )
	{
		
	}

private:
	unsigned int mWidth;
	unsigned int mHeight;
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

	unsigned char *mData;
};

class GIMaterial
{
public:
	GIMaterial() : mAlbedoTexture(NULL), mMaterialName(NULL), mTexturingType(ETT_COLOR) {}

	void SetMaterialName( const char *MaterialName )
	{
		SafeDeleteArray( &mMaterialName );
		if( MaterialName == NULL )
			return;
		mMaterialName = new char[strlen(MaterialName)+1];
		strcpy( mMaterialName, MaterialName );
	}
	char* GetMaterialName() { return mMaterialName; }

	void SetAlbedoTexture( GITexture *pTexture ) { mAlbedoTexture = pTexture; }
	GITexture* GetAlbedoTexture() { return mAlbedoTexture; }

	void SetAmbientColor( GIVector4 AmbientColor ) { mAmbientColor = AmbientColor; }
	void SetDiffuseColor( GIVector4 DiffuseColor ) { mDiffuseColor = DiffuseColor; }
	void SetSpecularColor( GIVector4 SpecularColor ) { mSpecularColor = SpecularColor; }
	void SetShininess( float Shininess ) { mShininess = Shininess; }

	GIVector4 GetAmbientColor() { return mAmbientColor; }
	GIVector4 GetDiffuseColor() { return mDiffuseColor; }
	GIVector4 GetSpecularColor() { return mSpecularColor; }
	float GetShininess() { return mShininess; }

	
	enum ETexturingType {
		ETT_REPLACE, ETT_MODULATE, ETT_COLOR, ETT_VERTEX_COLOR
	};

	ETexturingType mTexturingType;

private:
	GITexture *mAlbedoTexture;
	char *mMaterialName;

	GIVector4 mAmbientColor;
	GIVector4 mDiffuseColor;
	GIVector4 mSpecularColor;
	float mShininess;
};