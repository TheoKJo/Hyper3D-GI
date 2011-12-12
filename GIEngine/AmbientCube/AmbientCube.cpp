/**
 * \file		AmbientCube.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created		2011/11/22 21:09
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include "AmbientCube.h"
#include "../IrradianceVolume.h"

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
//	//Volume->mVolumeData = new AmbientCube[Volume->m_Size];
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

