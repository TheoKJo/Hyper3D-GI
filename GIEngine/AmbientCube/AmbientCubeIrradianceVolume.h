/*!
 * \file		AmbientCubeIrradianceVolume.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:12
 * copyright	Hybrid (creavol@gmail.com)
 */
#include "AmbientCube.h"
#include "../IrradianceVolume.h"

namespace GIEngine {

/*!
 * 
 * \date 2011.03.28
 * \author Hybrid	
 */
class AmbientCubeIrradianceVolume : public IrradianceVolume<AmbientCube>
{
public:
	AmbientCubeIrradianceVolume( int SizeX, int SizeY, int SizeZ, GIVector3 BoundingBoxMinVector, float CellLength )
		: IrradianceVolume<AmbientCube>( SizeX, SizeY, SizeZ, BoundingBoxMinVector, CellLength ), m_AmbientCubeArray(NULL)
	{
		m_AmbientCubeArray = new AmbientCube[m_Size];
	}
	~AmbientCubeIrradianceVolume()
	{
		SafeDeleteArray( &m_AmbientCubeArray );
	}

	static AmbientCubeIrradianceVolume* CreateIrradianceVolume( float CellLength, const GIVector3 &MinVector, const GIVector3 &MaxVector );

	static AmbientCubeIrradianceVolume* LoadFromFile( const char *strFilename );
	bool SaveToFile( const char *strFilename );

	AmbientCube& GetAmbientCube( int indexX, int indexY, int indexZ );
	AmbientCube& GetAmbientCube( int index );

	const AmbientCube& GetAmbientCube( int indexX, int indexY, int indexZ ) const;
	const AmbientCube& GetAmbientCube( int index ) const;

private:
	AmbientCube *m_AmbientCubeArray;
};
};