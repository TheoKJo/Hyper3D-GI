/*!
 * \file		SphericalHarmonics.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:16
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#ifndef SPHERICAL_HARMONICS_H
#define SPHERICAL_HARMONICS_H

#include <assert.h>

class SphericalHarmonicsAbs
{
public:
	virtual ~SphericalHarmonicsAbs() {}

protected:
	SphericalHarmonicsAbs( unsigned int order ) : mOrder(order) {}	

public:
	unsigned int GetOrder() const { return mOrder; }
	unsigned int GetNumberOfCoefficients() const { return mOrder*mOrder; }

private:
	unsigned int mOrder;
};

template<unsigned int order>
class SphericalHarmonics : public SphericalHarmonicsAbs
{
public:
	/*!
	 * 1st-order: 1 coefficients
	 * 2nd-order: 4 coefficients
	 * 3rd-order: 9 coefficients
	 * nth-order: n^2 coefficients
	 */
	SphericalHarmonics() : SphericalHarmonicsAbs(order) {}
	SphericalHarmonics( const SphericalHarmonics<order> &sh )
		: SphericalHarmonicsAbs(order)
	{
		for( unsigned int i = 0; i < GetNumberOfCoefficients(); i++ )
			coefficients[i] = sh.coefficients[i];
	}
	~SphericalHarmonics() {}

	static unsigned int GetOrder() { return order; }
	static unsigned int GetNumberOfCoefficients() { return order*order; }

	void Reset();

	float InnerProduct( const SphericalHarmonics &SH ) const;

	// component-wise operator
	SphericalHarmonics<order> operator+( SphericalHarmonics<order> sh ) const;
	SphericalHarmonics<order> operator+=( SphericalHarmonics<order> sh );
	SphericalHarmonics<order> operator*( float scalar ) const;

	//! 작은 order에 맞춰서 계산
	/*template<unsigned int order2>
	float DotProduct( const SphericalHarmonics<order2> &SH ) const;*/

	static float SH( int l, int m, float theta, float phi );

	float coefficients[order*order];

private:
	//float mCoefficientArray[order*order];
};

class SphericalHarmonicsAbsRGB
{
public:
	virtual ~SphericalHarmonicsAbsRGB() {}

protected:
	SphericalHarmonicsAbsRGB( unsigned int order ) : mOrder(order) {}

public:
	unsigned int GetOrder() const { return mOrder; }
	unsigned int GetNumberOfCoefficients() const { return mOrder*mOrder; }

private:
	unsigned int mOrder;
};

template<unsigned int order>
class SphericalHarmonicsRGB : public SphericalHarmonicsAbsRGB
{
public:
	SphericalHarmonicsRGB() : SphericalHarmonicsAbsRGB(order) {}
	SphericalHarmonicsRGB( const SphericalHarmonics<order> &_r, const SphericalHarmonics<order> &_g, const SphericalHarmonics<order> &_b )
		: SphericalHarmonicsAbsRGB(order), r(_r), g(_g), b(_b)
	{
	}
	~SphericalHarmonicsRGB() {}

	void Reset() { r.Reset(); g.Reset(); b.Reset(); }

	static unsigned int GetOrder() { return order; }
	static unsigned int GetNumberOfCoefficients() { return order*order; }

	// component-wise operator
	SphericalHarmonicsRGB<order> operator+( SphericalHarmonicsRGB<order> sh ) const;
	SphericalHarmonicsRGB<order> operator+=( SphericalHarmonicsRGB<order> sh );
	SphericalHarmonicsRGB<order> operator*( float scalar ) const;

//private:
	SphericalHarmonics<order> r;
	SphericalHarmonics<order> g;
	SphericalHarmonics<order> b;
};

#include "SphericalHarmonics.inl"

#endif

#include "SHMatrix.h"