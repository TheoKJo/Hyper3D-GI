#pragma once

namespace GIEngine { namespace Radiosity {

class RadiosityLightMapMaterial
{
public:
	RadiosityLightMapMaterial( unsigned TextureWidth, unsigned int TextureHeight )
		: mWidth(TextureWidth), mHeight(TextureHeight)
	{
		mQuadrangleNumList = new int[mWidth*mHeight];
		// TODO: assert mQuadrangleNumList != NULL

		for( unsigned int i = 0; i < mWidth*mHeight; i++ )
			mQuadrangleNumList[i] = -1;
	}

	unsigned int GetWidth() { return mWidth; }
	unsigned int GetHeight() { return mHeight; }

private:
	int *mQuadrangleNumList;
	unsigned int mWidth;
	unsigned int mHeight;
};

};};