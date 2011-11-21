#include "AmbientCube.h"
#include "IrradianceVolume.h"

/*!
 * 
 * \date 2011.03.28
 * \author Hybrid	
 */
class AmbientCubeIrradianceVolume : public IrradianceVolume<AmbientCube>
{
public:
	AmbientCubeIrradianceVolume( int SizeX, int SizeY, int SizeZ, GIVector3 BoundingBoxMinVector, float CellLength )
		: IrradianceVolume<AmbientCube>( SizeX, SizeY, SizeZ, BoundingBoxMinVector, CellLength ), mAmbientCubeArray(NULL)
	{
		mAmbientCubeArray = new AmbientCube[mSize];
	}
	~AmbientCubeIrradianceVolume()
	{
		SafeDeleteArray( &mAmbientCubeArray );
	}

	static AmbientCubeIrradianceVolume* CreateIrradianceVolume( float CellLength, const GIVector3 &MinVector, const GIVector3 &MaxVector );

	static AmbientCubeIrradianceVolume* LoadFromFile( const char *strFilename );
	bool SaveToFile( const char *strFilename );

	AmbientCube& GetAmbientCube( int indexX, int indexY, int indexZ );
	AmbientCube& GetAmbientCube( int index );

	const AmbientCube& GetAmbientCube( int indexX, int indexY, int indexZ ) const;
	const AmbientCube& GetAmbientCube( int index ) const;

private:
	AmbientCube *mAmbientCubeArray;
};