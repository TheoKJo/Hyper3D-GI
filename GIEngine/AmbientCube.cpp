#include "GIEnginePCH.h"

#include "AmbientCube.h"
#include "IrradianceVolume.h"

//inline IrradianceVolume<AmbientCube>* IrradianceVolume<AmbientCube>::LoadFromFile( const char *strFilename )
//{
//	std::fstream fs( strFilename, std::ios_base::in );
//	if( !fs.is_open() )
//		return NULL;
//
//	//int Size, SizeX, SizeY, SizeZ;
//
//	IrradianceVolume *Volume = new IrradianceVolume<AmbientCube>;
//
//	fs >> Volume->mSize;
//	fs >> Volume->mSizeX;
//	fs >> Volume->mSizeY;
//	fs >> Volume->mSizeZ;
//
//	fs >> Volume->mBoundingBoxMinVector.x;
//	fs >> Volume->mBoundingBoxMinVector.y;
//	fs >> Volume->mBoundingBoxMinVector.z;
//	fs >> Volume->mCellLength;
//
//	if( Volume->mSize == 0 )
//	{
//		delete Volume;
//		return NULL;
//	}
//
//	if( Volume->mSize != Volume->mSizeX*Volume->mSizeY*Volume->mSizeZ )
//	{
//		delete Volume;
//		return NULL;
//	}
//
//	//Volume->mVolumeData = new AmbientCube[Volume->mSize];
//
//#ifdef IRRADIANCE_VOLUME_PRECALCULATED_POSITION
//	assert( false );
//#else
//
//	// TODO : 완전 임시
//	for( int i = 0; i < Volume->mSize; i++ )
//	{
//		int index = 0;
//		fs >> index;
//		T *ac = &Volume->mVolumeData[index];
//		for( int k = 0; k < 6; k++ )
//		{
//			GIVector3 Color;
//			fs >> Color.r;
//			fs >> Color.g;
//			fs >> Color.b;
//			ac->GetIrradianceArray()[k].r = Color.r;
//			ac->GetIrradianceArray()[k].g = Color.g;
//			ac->GetIrradianceArray()[k].b = Color.b;
//		}
//	}
//#endif
//
//	fs.close();
//	return Volume;
//}