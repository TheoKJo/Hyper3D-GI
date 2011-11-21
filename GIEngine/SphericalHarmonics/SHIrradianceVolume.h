#include "SphericalHarmonics.h"
#include "../IrradianceVolume.h"
//#include "IrradianceVolume.h"

/*!
 * 
 * \date 2011.03.28
 * \author Hybrid	
 */
class SHIrradianceVolume : public IrradianceVolume<SphericalHarmonicsRGB<3> >
{
public:
	~SHIrradianceVolume();

	static SHIrradianceVolume* CreateIrradianceVolume( float CellLength, const GIVector3 &MinVector, const GIVector3 &MaxVector );
	static SHIrradianceVolume* LoadFromFile( const char *strFilename );
	bool SaveToFile( const char *strFilename );

	const SphericalHarmonicsRGB<3>& GetSH_RGB( int index ) const;
	SphericalHarmonicsRGB<3> GetSH_RGB( int index );
	SphericalHarmonicsRGB<3>* GetSH_RGBPtr( int index );

private:
	SHIrradianceVolume();
	SHIrradianceVolume( int SizeX, int SizeY, int SizeZ, const GIVector3& BoundingBoxMinVector, float CellLength );	

private:
	unsigned int mMaxSHOrder;
	SphericalHarmonicsRGB<3> *mSHVolume;
};