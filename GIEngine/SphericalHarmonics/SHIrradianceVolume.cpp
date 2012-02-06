/*!
 * \file		SHIrradianceVolume.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:16
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "GIEnginePCH.h"
#include "SHIrradianceVolume.h"

using namespace GIEngine;


SHIrradianceVolume* SHIrradianceVolume::CreateIrradianceVolume( float CellLength, const GIVector3 &MinVector, const GIVector3 &MaxVector )
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
	return new SHIrradianceVolume( SizeX, SizeY, SizeZ, VolumeMinVector, CellLength );
}

SHIrradianceVolume* SHIrradianceVolume::LoadFromFile( const char *strFilename )
{
	std::fstream fs( strFilename, std::ios_base::in );
	if( !fs.is_open() )
		return NULL;

	SHIrradianceVolume *Volume = new SHIrradianceVolume;
	
	fs >> Volume->m_Size;
	fs >> Volume->m_SizeX;
	fs >> Volume->m_SizeY;
	fs >> Volume->m_SizeZ;

	fs >> Volume->m_BoundingBoxMinVector.x;
	fs >> Volume->m_BoundingBoxMinVector.y;
	fs >> Volume->m_BoundingBoxMinVector.z;
	fs >> Volume->m_CellLength;

	if( Volume->m_Size == 0 )
	{
		delete Volume;
		return NULL;
	}

	if( Volume->m_Size != Volume->m_SizeX*Volume->m_SizeY*Volume->m_SizeZ )
	{
		delete Volume;
		return NULL;
	}

	Volume->mSH = new SphericalHarmonicsRGB<3>[Volume->m_Size];

	for( int i = 0; i < Volume->m_Size; i++ )
	{
		int index = 0;
		fs >> index;
		SphericalHarmonicsRGB<3> *sh_rgb = Volume->GetSH_RGBPtr( index );
		unsigned int NumberOfCoefficients = sh_rgb->GetNumberOfCoefficients();

		for( unsigned int k = 0; k < NumberOfCoefficients; k++ )
		{
			float coefficient;
			fs >> coefficient;
			sh_rgb->r.coefficients[k] = coefficient;
		}

		for( unsigned int k = 0; k < NumberOfCoefficients; k++ )
		{
			float coefficient;
			fs >> coefficient;
			sh_rgb->g.coefficients[k] = coefficient;
		}

		for( unsigned int k = 0; k < NumberOfCoefficients; k++ )
		{
			float coefficient;
			fs >> coefficient;
			sh_rgb->b.coefficients[k] = coefficient;
		}
	}

	fs.close();
	return Volume;
}

bool SHIrradianceVolume::SaveToFile( const char *strFilename )
{
	std::fstream fs( strFilename, std::ios_base::out );
	if( !fs.is_open() )
		return false;

	if( m_Size == 0 )
		return false;

	fs << m_Size << '\t';
	fs << m_SizeX << '\t';
	fs << m_SizeY << '\t';
	fs << m_SizeZ << std::endl;

	fs << m_BoundingBoxMinVector.x << '\t';
	fs << m_BoundingBoxMinVector.y << '\t';
	fs << m_BoundingBoxMinVector.z << std::endl;
	fs << m_CellLength << std::endl;

	for( int i = 0; i < m_Size; i++ )
	{
		fs << i << std::endl;
		SphericalHarmonicsRGB<3> &sh_rgb = GetSH_RGB( i );

		unsigned int NumberOfCoefficients = sh_rgb.GetNumberOfCoefficients();

		for( unsigned int k = 0; k < NumberOfCoefficients; k++ )
			fs << sh_rgb.r.coefficients[k] << '\t';
		fs << std::endl;

		for( unsigned int k = 0; k < NumberOfCoefficients; k++ )
			fs << sh_rgb.g.coefficients[k] << '\t';
		fs << std::endl;

		for( unsigned int k = 0; k < NumberOfCoefficients; k++ )
			fs << sh_rgb.b.coefficients[k] << '\t';
		fs << std::endl;
	}

	fs.close();
	return true;
}

const SphericalHarmonicsRGB<3>& SHIrradianceVolume::GetSH_RGB( int index ) const
{
	return mSH[index];
}

SphericalHarmonicsRGB<3> SHIrradianceVolume::GetSH_RGB( int index )
{
	return mSH[index];
}

SphericalHarmonicsRGB<3>* SHIrradianceVolume::GetSH_RGBPtr( int index )
{
	return &mSH[index];
}

SHIrradianceVolume::SHIrradianceVolume()
	: mSH(NULL)
{
}

SHIrradianceVolume::SHIrradianceVolume( int SizeX, int SizeY, int SizeZ, const GIVector3& BoundingBoxMinVector, float CellLength )
	: IrradianceVolume<SphericalHarmonicsRGB<3> >( SizeX, SizeY, SizeZ, BoundingBoxMinVector, CellLength )
{
	mSH = new SphericalHarmonicsRGB<3>[m_Size];
}

SHIrradianceVolume::~SHIrradianceVolume()
{
	SafeDeleteArray( &mSH );
}