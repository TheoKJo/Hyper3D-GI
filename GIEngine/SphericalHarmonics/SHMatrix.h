/*!
 * \file		SHMatrix.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:16
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#ifndef SPHERICAL_HARMONICS_MATRIX_H
#define SPHERICAL_HARMONICS_MATRIX_H

#ifndef SPHERICAL_HARMONICS_H
#error You should include "SphericalHarmonics.h" only.
#include "SphericalHarmonics.h"
#endif

#include <math.h>

namespace GIEngine {

template<unsigned int rowOrder, unsigned int columnOrder>
class SHMatrix
{
public:
	SHMatrix() : mRowOrder(rowOrder), mColumnOrder(columnOrder)
	{
		
	}

	SHMatrix( float *elementArray ) : mRowOrder(rowOrder), mColumnOrder(columnOrder)
	{
		for( unsigned int i = 0; i < GetNumberOfElements(); i++ )
			mElements[i] = elementArray[i];
	}

	SHMatrix( const SHMatrix<rowOrder,columnOrder> &matrix ) : mRowOrder(rowOrder), mColumnOrder(columnOrder)
	{
		for( unsigned int i = 0; i < GetNumberOfElements(); i++ )
			mElements[i] = matrix.mElements[i];
	}

	void MakeIdentity()
	{
		// TODO: 성능은??
		for( unsigned int j = 0; j < GetNumberOfRows(); j++ )
			for( unsigned int i = 0; i < GetNumberOfColumns(); i++ )
		{
			mElements[i*GetNumberOfColumns()+j] = (i == j?1.0f:0.0f);
		}
	}

	void SetElement( unsigned int rowIndex, unsigned int columnIndex, float element )
	{
		mElements[rowIndex*GetNumberOfColumns() + columnIndex] = element;
	}

	float GetElement( unsigned int rowIndex, unsigned int columnIndex ) const
	{
		return mElements[rowIndex*GetNumberOfColumns() + columnIndex];
	}

	void SetRow( unsigned int rowIndex, float *elementArray )
	{
		// TODO: 컴파일러에서 상수로 만들어주는지 확인
		for( unsigned int i = 0; i < GetNumberOfColumns(); i++ )
			mElements[rowIndex*GetNumberOfColumns() + i] = elementArray[i];
	}
	void SetColumn( unsigned int columnIndex, float *elementArray )
	{
		// TODO: 컴파일러에서 상수로 만들어주는지 확인
		for( unsigned int i = 0; i < GetNumberOfRows(); i++ )
			mElements[i*GetNumberOfColumns() + columnIndex] = elementArray[i];
	}

	SphericalHarmonics<columnOrder> operator*( const SphericalHarmonics<rowOrder> &sh_vec ) const
	{
		SphericalHarmonics<columnOrder> retVector;
		for( unsigned int r = 0; r < GetNumberOfRows(); r++ )
		{
			retVector.coefficients[r] = 0.0f;
			for( unsigned int c = 0; c < GetNumberOfColumns(); c++ )
			{
				retVector.coefficients[r] += this->GetElement( r, c ) * sh_vec.coefficients[c];
			}
		}
		return retVector;
	}

	// TODO: 속도 개선?
	template<unsigned int columnOrder2>
	SHMatrix<rowOrder,columnOrder2> operator*( const SHMatrix<columnOrder,columnOrder2> &matrix ) const
	{
		SHMatrix<rowOrder,columnOrder2> retMatrix;

		unsigned int rowCount = SphericalHarmonics<rowOrder>::GetNumberOfCoefficients();
		unsigned int columnCount = SphericalHarmonics<columnOrder>::GetNumberOfCoefficients();
		unsigned int column2Count = SphericalHarmonics<columnOrder2>::GetNumberOfCoefficients();

		for( unsigned int r = 0; r < rowCount; r++ )
			for( unsigned int c = 0; c < column2Count; c++ )
		{
			float element = 0.0f;
			for( unsigned int i = 0; i < columnCount; i++ )
				element += this->GetElement( r, i ) * matrix.GetElement( i, c );
			retMatrix.SetElement( r, c, element );
		}
		return retMatrix;
	}

private:
	unsigned int GetRowOrder() const { return mRowOrder; }
	unsigned int GetColumnOrder() const { return mColumnOrder; }
	unsigned int GetNumberOfRows() const { return mRowOrder*mRowOrder; }
	unsigned int GetNumberOfColumns() const { return mColumnOrder*mColumnOrder; }
	unsigned int GetNumberOfElements() const { return mRowOrder*mRowOrder * mColumnOrder*mColumnOrder; }

private:
	unsigned int mRowOrder;
	unsigned int mColumnOrder;

	// row-major order
	float mElements[rowOrder*rowOrder * columnOrder*columnOrder];
};
};

#endif