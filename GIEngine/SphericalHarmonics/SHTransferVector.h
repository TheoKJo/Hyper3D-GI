/*!
 * \file		SHTransferVector.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:16
 * copyright	Hybrid (creavol@gmail.com)
 */
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