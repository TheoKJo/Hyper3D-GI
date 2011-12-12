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
		: m_Size(0), m_SizeX(0), m_SizeY(0), m_SizeZ(0), m_BoundingBoxMinVector(0.0f, 0.0f, 0.0f), m_CellLength(0.0f)
	{}
	IrradianceVolume( int SizeX, int SizeY, int SizeZ, 
		GIVector3 BoundingBoxMinVector, float CellLength )
		: m_SizeX(SizeX), m_SizeY(SizeY), m_SizeZ(SizeZ), 
		m_BoundingBoxMinVector(BoundingBoxMinVector), m_CellLength(CellLength)
	{
		m_Size = m_SizeX*m_SizeY*m_SizeZ;

		//mVolumeData = new T[m_Size];
	}

	virtual ~IrradianceVolume()
	{
		//delete[] mVolumeData;
	}

	bool IsInside( const GIVector3 &Position ) const
	{
		int IndexX = int( (Position.x - m_BoundingBoxMinVector.x)/m_CellLength );
		if( IndexX < 0 )
			return false;
		else if( m_SizeX <= IndexX )
			return false;
		int IndexY = int( (Position.y - m_BoundingBoxMinVector.y)/m_CellLength );
		if( IndexY < 0 )
			return false;
		else if( m_SizeY <= IndexY )
			return false;
		int IndexZ = int( (Position.z - m_BoundingBoxMinVector.z)/m_CellLength );
		if( IndexZ < 0 )
			return false;
		else if( m_SizeZ <= IndexZ )
			return false;

		return true;
	}

	//! 반올림
	int GetNearestCell( const GIVector3 &Position ) const
	{
		int IndexX = int( (Position.x - m_BoundingBoxMinVector.x)/m_CellLength + 0.5f );
		if( IndexX < 0 )
			IndexX = 0;
		else if( m_SizeX <= IndexX )
			IndexX = m_SizeX-1;
		int IndexY = int( (Position.y - m_BoundingBoxMinVector.y)/m_CellLength + 0.5f );
		if( IndexY < 0 )
			IndexY = 0;
		else if( m_SizeY <= IndexY )
			IndexY = m_SizeY-1;
		int IndexZ = int( (Position.z - m_BoundingBoxMinVector.z)/m_CellLength + 0.5f );
		if( IndexZ < 0 )
			IndexZ = 0;
		else if( m_SizeZ <= IndexZ )
			IndexZ = m_SizeZ-1;

		return GetIndex( IndexX, IndexY, IndexZ );
	}

	//! 반올림 아님
	void GetIntegerIndex( const GIVector3 &Position, int *outX, int *outY, int *outZ ) const
	{
		*outX = int((Position.x - m_BoundingBoxMinVector.x)/m_CellLength);
		*outY = int((Position.y - m_BoundingBoxMinVector.y)/m_CellLength);
		*outZ = int((Position.z - m_BoundingBoxMinVector.z)/m_CellLength);
	}

	void GetFloatIndex( const GIVector3 &Position, float *outX, float *outY, float *outZ ) const
	{
		*outX = (Position.x - m_BoundingBoxMinVector.x)/m_CellLength;
		*outY = (Position.y - m_BoundingBoxMinVector.y)/m_CellLength;
		*outZ = (Position.z - m_BoundingBoxMinVector.z)/m_CellLength;
	}

	int GetIndex( int x, int y, int z ) const
	{
		assert( 0 <= x && x < m_SizeX );
		assert( 0 <= y && y < m_SizeY );
		assert( 0 <= z && z < m_SizeZ );
		return x + y * m_SizeX + z * m_SizeX * m_SizeY;
	}

	//! 중간
	GIVector3 GetPosition( int x, int y, int z ) const
	{
		return GIVector3( 
			m_BoundingBoxMinVector.x + (float(x)+0.5f) * m_CellLength, 
			m_BoundingBoxMinVector.y + (float(y)+0.5f) * m_CellLength, 
			m_BoundingBoxMinVector.z + (float(z)+0.5f) * m_CellLength );
	}

	static IrradianceVolume<T>* LoadFromFile( const char *strFilename );
	bool SaveToFile( const char *strFilename );

	int GetSize() const { return m_Size; }
	int GetSizeX() const { return m_SizeX; }
	int GetSizeY() const { return m_SizeY; }
	int GetSizeZ() const { return m_SizeZ; }

	T* GetVolumeArray() { return mVolumeData; }

protected:
	int m_Size;
	int m_SizeX;
	int m_SizeY;
	int m_SizeZ;

protected:
	GIVector3 m_BoundingBoxMinVector;
	float m_CellLength;

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
//	fs >> Volume->m_Size;
//	fs >> Volume->m_SizeX;
//	fs >> Volume->m_SizeY;
//	fs >> Volume->m_SizeZ;
//
//	fs >> Volume->m_BoundingBoxMinVector.x;
//	fs >> Volume->m_BoundingBoxMinVector.y;
//	fs >> Volume->m_BoundingBoxMinVector.z;
//	fs >> Volume->m_CellLength;
//
//	if( Volume->m_Size == 0 )
//	{
//		delete Volume;
//		return NULL;
//	}
//
//	if( Volume->m_Size != Volume->m_SizeX*Volume->m_SizeY*Volume->m_SizeZ )
//	{
//		delete Volume;
//		return NULL;
//	}
//
//	Volume->mVolumeData = new T[Volume->m_Size];
//
//#ifdef IRRADIANCE_VOLUME_PRECALCULATED_POSITION
//	assert( false );
//#else
//
//	// TODO : 완전 임시
//	for( int i = 0; i < Volume->m_Size; i++ )
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
//	fs << m_Size << '\t' << m_SizeX << '\t' << m_SizeY << '\t' << m_SizeZ << std::endl;
//	fs << m_BoundingBoxMinVector.x << '\t' << m_BoundingBoxMinVector.y << '\t' << m_BoundingBoxMinVector.z << std::endl;
//	fs << m_CellLength << std::endl;
//
//#ifdef IRRADIANCE_VOLUME_PRECALCULATED_POSITION
//	assert( false );
//#else
//
//	// TODO : 완전 임시
//	for( int i = 0; i < m_Size; i++ )
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