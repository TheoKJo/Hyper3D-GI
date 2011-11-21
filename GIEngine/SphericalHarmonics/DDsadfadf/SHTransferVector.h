#pragma once

#include "SphericalHarmonics.h"

typedef SphericalHarmonics SHTransferVector;

class SHTransferMatrix
{
public:
	SHTransferMatrix( unsigned int row, unsigned column )
	{
		mSphericalHarmonicsArray = SphericalHarmonics( column )[row];
	}
	~SHTransferMatrix()
	{
		SafeDeleteArray( &mSphericalHarmonicsArray );
	}

private:
	SphericalHarmonics *mSphericalHarmonicsArray;
};

//template<unsigned int order>
//class SHTransferVector
//{
//	
//};