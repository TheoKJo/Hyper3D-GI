/*!
 * \file		IrradianceVolume.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:16
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "GIEnginePCH.h"

#include <fstream>

//#define IRRADIANCE_VOLUME_PRECALCULATED_POSITION

/*!
 * 
 * \date 2010.12.06
 * \author Hybrid	
 */
template<class T>
class IrradianceVolume
{
public:
	IrradianceVolume()
		: mSize(0), mSizeX(0), mSizeY(0), mSizeZ(0), mBoundingBoxMinVector(0.0f, 0.0f, 0.0f), mCellLength(0.0f)
	{}
	IrradianceVolume( int SizeX, int SizeY, int SizeZ, 
		GIVector3 BoundingBoxMinVector, float CellLength )
		: mSizeX(SizeX), mSizeY(SizeY), mSizeZ(SizeZ), 
		mBoundingBoxMinVector(BoundingBoxMinVector), mCellLength(CellLength)
	{
		mSize = mSizeX*mSizeY*mSizeZ;

		//mVolumeData = new T[mSize];
	}

	virtual ~IrradianceVolume()
	{
		//delete[] mVolumeData;
	}

	bool IsInside( const GIVector3 &Position ) const
	{
		int IndexX = int( (Position.x - mBoundingBoxMinVector.x)/mCellLength );
		if( IndexX < 0 )
			return false;
		else if( mSizeX <= IndexX )
			return false;
		int IndexY = int( (Position.y - mBoundingBoxMinVector.y)/mCellLength );
		if( IndexY < 0 )
			return false;
		else if( mSizeY <= IndexY )
			return false;
		int IndexZ = int( (Position.z - mBoundingBoxMinVector.z)/mCellLength );
		if( IndexZ < 0 )
			return false;
		else if( mSizeZ <= IndexZ )
			return false;

		return true;
	}

	//! 반올림
	int GetNearestCell( const GIVector3 &Position ) const
	{
		int IndexX = int( (Position.x - mBoundingBoxMinVector.x)/mCellLength + 0.5f );
		if( IndexX < 0 )
			IndexX = 0;
		else if( mSizeX <= IndexX )
			IndexX = mSizeX-1;
		int IndexY = int( (Position.y - mBoundingBoxMinVector.y)/mCellLength + 0.5f );
		if( IndexY < 0 )
			IndexY = 0;
		else if( mSizeY <= IndexY )
			IndexY = mSizeY-1;
		int IndexZ = int( (Position.z - mBoundingBoxMinVector.z)/mCellLength + 0.5f );
		if( IndexZ < 0 )
			IndexZ = 0;
		else if( mSizeZ <= IndexZ )
			IndexZ = mSizeZ-1;

		return GetIndex( IndexX, IndexY, IndexZ );
	}

	//! 반올림 아님
	void GetIntegerIndex( const GIVector3 &Position, int *outX, int *outY, int *outZ ) const
	{
		*outX = int((Position.x - mBoundingBoxMinVector.x)/mCellLength);
		*outY = int((Position.y - mBoundingBoxMinVector.y)/mCellLength);
		*outZ = int((Position.z - mBoundingBoxMinVector.z)/mCellLength);
	}

	void GetFloatIndex( const GIVector3 &Position, float *outX, float *outY, float *outZ ) const
	{
		*outX = (Position.x - mBoundingBoxMinVector.x)/mCellLength;
		*outY = (Position.y - mBoundingBoxMinVector.y)/mCellLength;
		*outZ = (Position.z - mBoundingBoxMinVector.z)/mCellLength;
	}

	int GetIndex( int x, int y, int z ) const
	{
		assert( 0 <= x && x < mSizeX );
		assert( 0 <= y && y < mSizeY );
		assert( 0 <= z && z < mSizeZ );
		return x + y * mSizeX + z * mSizeX * mSizeY;
	}

	//! 중간
	GIVector3 GetPosition( int x, int y, int z ) const
	{
		return GIVector3( 
			mBoundingBoxMinVector.x + (float(x)+0.5f) * mCellLength, 
			mBoundingBoxMinVector.y + (float(y)+0.5f) * mCellLength, 
			mBoundingBoxMinVector.z + (float(z)+0.5f) * mCellLength );
	}

	static IrradianceVolume<T>* LoadFromFile( const char *strFilename );
	bool SaveToFile( const char *strFilename );

	int GetSize() const { return mSize; }
	int GetSizeX() const { return mSizeX; }
	int GetSizeY() const { return mSizeY; }
	int GetSizeZ() const { return mSizeZ; }

	T* GetVolumeArray() { return mVolumeData; }

protected:
	int mSize;
	int mSizeX;
	int mSizeY;
	int mSizeZ;

protected:
	GIVector3 mBoundingBoxMinVector;
	float mCellLength;

	//T *mVolumeData;
};

//template<class T>
//inline IrradianceVolume<T>* IrradianceVolume<T>::LoadFromFile( const char *strFilename )
//{
//	std::fstream fs( strFilename, std::ios_base::in );
//	if( !fs.is_open() )
//		return NULL;
//
//	//int Size, SizeX, SizeY, SizeZ;
//
//	IrradianceVolume *Volume = new IrradianceVolume<T>;
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
//	Volume->mVolumeData = new T[Volume->mSize];
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

//template<class T>
//inline bool IrradianceVolume<T>::SaveToFile( const char *strFilename )
//{
//	std::fstream fs( strFilename, std::ios_base::out );
//	if( !fs.is_open() )
//		return false;
//
//	fs << mSize << '\t' << mSizeX << '\t' << mSizeY << '\t' << mSizeZ << std::endl;
//	fs << mBoundingBoxMinVector.x << '\t' << mBoundingBoxMinVector.y << '\t' << mBoundingBoxMinVector.z << std::endl;
//	fs << mCellLength << std::endl;
//
//#ifdef IRRADIANCE_VOLUME_PRECALCULATED_POSITION
//	assert( false );
//#else
//
//	// TODO : 완전 임시
//	for( int i = 0; i < mSize; i++ )
//	{
//		T *ac = &mVolumeData[i];
//		fs << i << std::endl;
//		for( int k = 0; k < 6; k++ )
//			fs << ac->GetIrradiance( k ).r << '\t' << ac->GetIrradiance( k ).g << '\t' << ac->GetIrradiance( k ).b << std::endl;
//	}
//#endif
//
//	fs.close();
//	return true;
//}