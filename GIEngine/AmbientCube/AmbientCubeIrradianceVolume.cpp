/*!
 * \file		AmbientCubeIrradianceVolume.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:12
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"

#include <assert.h>
#include "AmbientcubeIrradianceVolume.h"

AmbientCubeIrradianceVolume* AmbientCubeIrradianceVolume::CreateIrradianceVolume( float CellLength, const GIVector3 &MinVector, const GIVector3 &MaxVector )
{
	GIVector3 DeltaVector = MaxVector - MinVector;

	int SizeX = int( ceil( DeltaVector.x / CellLength ) );
	int SizeY = int( ceil( DeltaVector.y / CellLength ) );
	int SizeZ = int( ceil( DeltaVector.z / CellLength ) );

	assert( SizeX >= 0 && SizeY >= 0 && SizeZ >= 0 );

	float MarginX = DeltaVector.x - float(SizeX)*CellLength;
	float MarginY = DeltaVector.y - float(SizeY)*CellLength;
	float MarginZ = DeltaVector.z - float(SizeZ)*CellLength;

	// ¿ÀÂ÷
	if( MarginX < 0.0f ) MarginX = 0.0f; if( CellLength < MarginX ) MarginX = CellLength;
	if( MarginY < 0.0f ) MarginY = 0.0f; if( CellLength < MarginY ) MarginY = CellLength;
	if( MarginZ < 0.0f ) MarginZ = 0.0f; if( CellLength < MarginZ ) MarginZ = CellLength;

	GIVector3 VolumeMinVector( MarginX * 0.5f + MinVector.x, MarginY * 0.5f + MinVector.y, MarginZ * 0.5f + MinVector.z );
	assert( MinVector.x <= VolumeMinVector.x && MinVector.y <= VolumeMinVector.y && MinVector.z <= VolumeMinVector.z );
	/*NIASSERT( 
		SizeX * CellLength + MarginX * 0.5f == DeltaVector.x && 
		MinVector.y <= VolumeMinVector.y && MinVector.z <= VolumeMinVector.z );*/

	AmbientCubeIrradianceVolume* AmbientCubeVolume = new AmbientCubeIrradianceVolume( SizeX, SizeY, SizeZ, VolumeMinVector, CellLength );
	return AmbientCubeVolume;
}

AmbientCubeIrradianceVolume* AmbientCubeIrradianceVolume::LoadFromFile( const char *strFilename )
{
	std::fstream fs( strFilename, std::ios_base::in );
	if( !fs.is_open() )
		return NULL;

	unsigned int Size, SizeX, SizeY, SizeZ;
	GIVector3 BoundingBoxMinVector;
	float CellLength;

	fs >> Size;
	fs >> SizeX;
	fs >> SizeY;
	fs >> SizeZ;

	if( Size == 0 || SizeX*SizeY*SizeZ != Size )
		return NULL;

	fs >> BoundingBoxMinVector.x;
	fs >> BoundingBoxMinVector.y;
	fs >> BoundingBoxMinVector.z;
	fs >> CellLength;

	AmbientCubeIrradianceVolume *Volume = new AmbientCubeIrradianceVolume( SizeX, SizeY, SizeZ, BoundingBoxMinVector, CellLength );

	for( int i = 0; i < Volume->mSize; i++ )
	{
		int index = 0;
		fs >> index;
		AmbientCube &ac = Volume->GetAmbientCube( index );
		for( int k = 0; k < 6; k++ )
		{
			GIVector3 Color;
			fs >> Color.r;
			fs >> Color.g;
			fs >> Color.b;
			ac.GetIrradianceArray()[k].r = Color.r;
			ac.GetIrradianceArray()[k].g = Color.g;
			ac.GetIrradianceArray()[k].b = Color.b;
		}
	}

	fs.close();
	return Volume;
}

bool AmbientCubeIrradianceVolume::SaveToFile( const char *strFilename )
{
	std::fstream fs( strFilename, std::ios_base::out );
	if( !fs.is_open() )
		return false;

	if( mSize == 0 || mSizeX*mSizeY*mSizeZ != mSize )
		return false;

	fs << mSize << '\t';
	fs << mSizeX << '\t';
	fs << mSizeY << '\t';
	fs << mSizeZ << std::endl;

	fs << mBoundingBoxMinVector.x << '\t';
	fs << mBoundingBoxMinVector.y << '\t';
	fs << mBoundingBoxMinVector.z << '\t';
	fs << mCellLength << std::endl;

	for( int i = 0; i < mSize; i++ )
	{
		fs << i << '\t';
		const AmbientCube &ac = GetAmbientCube( i );
		for( int k = 0; k < 6; k++ )
		{
			const GIVector3 &Irradiance = ac.GetIrradiance( k );
			fs << Irradiance.r << '\t';
			fs << Irradiance.g << '\t';
			fs << Irradiance.b << '\t';
		}
		fs << std::endl;
	}

	fs.close();
	return true;
}

AmbientCube& AmbientCubeIrradianceVolume::GetAmbientCube( int indexX, int indexY, int indexZ )
{
	return GetAmbientCube( GetIndex( indexX, indexY, indexZ ) );
}

AmbientCube& AmbientCubeIrradianceVolume::GetAmbientCube( int index )
{
	assert( index < mSize );
	return mAmbientCubeArray[index];
}

const AmbientCube& AmbientCubeIrradianceVolume::GetAmbientCube( int indexX, int indexY, int indexZ ) const
{
	return GetAmbientCube( this->GetIndex( indexX, indexY, indexZ ) );
}

const AmbientCube& AmbientCubeIrradianceVolume::GetAmbientCube( int index ) const
{
	assert( index < mSize );
	return mAmbientCubeArray[index];
}