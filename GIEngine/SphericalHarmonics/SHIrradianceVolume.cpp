#include "GIEnginePCH.h"
#include "SHIrradianceVolume.h"

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
	
	fs >> Volume->mSize;
	fs >> Volume->mSizeX;
	fs >> Volume->mSizeY;
	fs >> Volume->mSizeZ;

	fs >> Volume->mBoundingBoxMinVector.x;
	fs >> Volume->mBoundingBoxMinVector.y;
	fs >> Volume->mBoundingBoxMinVector.z;
	fs >> Volume->mCellLength;

	if( Volume->mSize == 0 )
	{
		delete Volume;
		return NULL;
	}

	if( Volume->mSize != Volume->mSizeX*Volume->mSizeY*Volume->mSizeZ )
	{
		delete Volume;
		return NULL;
	}

	Volume->mSHVolume = new SphericalHarmonicsRGB<3>[Volume->mSize];

	for( int i = 0; i < Volume->mSize; i++ )
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

	if( mSize == 0 )
		return false;

	fs << mSize << '\t';
	fs << mSizeX << '\t';
	fs << mSizeY << '\t';
	fs << mSizeZ << std::endl;

	fs << mBoundingBoxMinVector.x << '\t';
	fs << mBoundingBoxMinVector.y << '\t';
	fs << mBoundingBoxMinVector.z << std::endl;
	fs << mCellLength << std::endl;

	for( int i = 0; i < mSize; i++ )
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
	return mSHVolume[index];
}

SphericalHarmonicsRGB<3> SHIrradianceVolume::GetSH_RGB( int index )
{
	return mSHVolume[index];
}

SphericalHarmonicsRGB<3>* SHIrradianceVolume::GetSH_RGBPtr( int index )
{
	return &mSHVolume[index];
}

SHIrradianceVolume::SHIrradianceVolume()
	: mSHVolume(NULL)
{
}

SHIrradianceVolume::SHIrradianceVolume( int SizeX, int SizeY, int SizeZ, const GIVector3& BoundingBoxMinVector, float CellLength )
	: IrradianceVolume<SphericalHarmonicsRGB<3> >( SizeX, SizeY, SizeZ, BoundingBoxMinVector, CellLength )
{
	mSHVolume = new SphericalHarmonicsRGB<3>[mSize];
}

SHIrradianceVolume::~SHIrradianceVolume()
{
	SafeDeleteArray( &mSHVolume );
}