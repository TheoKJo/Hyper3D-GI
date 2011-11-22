/*!
 * \file		SampleArray.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:17
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "../GIEngine/SphericalHarmonics/SphericalHarmonics.h"

class SampleArray
{
public:

};

class SphericalSampleArray : public SampleArray
{
public:
	SphericalSampleArray( unsigned int RayCount, unsigned int SampleArrayTypeFlag )
		: mRayCount(RayCount), mSampleArrayTypeFlag(SampleArrayTypeFlag)
		, mRayArray(NULL), mThetaArray(NULL), mPhiArray(NULL), mWeightArray(NULL)
	{
		// TODO: exception °í·Á
		if( (mSampleArrayTypeFlag & SA_RAY_ARRAY) > 0 )
			mRayArray = new GIRay[mRayCount];
		if( (mSampleArrayTypeFlag & SA_THETA_ARRAY) > 0 )
			mThetaArray = new float[mRayCount];
		if( (mSampleArrayTypeFlag & SA_PHI_ARRAY) > 0 )
			mPhiArray = new float[mRayCount];
		if( (mSampleArrayTypeFlag & SA_WEIGHT_ARRAY) > 0 )
			mWeightArray = new float[mRayCount];
	}
	virtual ~SphericalSampleArray()
	{
		if( mRayArray )
			delete[] mRayArray;
		if( mThetaArray )
			delete[] mThetaArray;
		if( mPhiArray )
			delete[] mPhiArray;
		if( mWeightArray )
			delete[] mWeightArray;
	}

	unsigned int GetRayCount() { return mRayCount; }
	GIRay* GetRayArray() { return mRayArray; }
	float* GetThetaArray() { return mThetaArray; }
	float* GetPhiArray() { return mPhiArray; }
	float* GetWeightArray() { return mWeightArray; }

	enum SPHERICAL_SAMPLE_ARRAY {
		SA_NONE			= 0, 
		SA_RAY_ARRAY	= 1, 
		SA_THETA_ARRAY	= 2, 
		SA_PHI_ARRAY	= 4, 
		SA_WEIGHT_ARRAY	= 8
	};

private:
	unsigned int mRayCount;
	int mSampleArrayTypeFlag;
	GIRay *mRayArray;
	float *mThetaArray;		//!< [0,  PI]
	float *mPhiArray;		//!< [0, 2PI)
	float *mWeightArray;
};

template<unsigned int order>
class SphericalHarmonicsSampleArray : public SphericalSampleArray
{
public:
	SphericalHarmonicsSampleArray( unsigned int RayCount, unsigned int SampleArrayType )
		: SphericalSampleArray( RayCount, SampleArrayType ), mSphericalHarmonicsArray(NULL)
	{
		if( (SampleArrayType&SA_SH_ARRAY) > 0 )
			mSphericalHarmonicsArray = new SphericalHarmonics[RayCount];
	}
	virtual ~SphericalHarmonicsSampleArray()
	{
		if( mSphericalHarmonicsArray )
			delete[] mSphericalHarmonicsArray;
	}

	SphericalHarmonics<order>* GetSHArray()
	{
		return mSphericalHarmonicsArray;
	}

	enum SPHERICAL_HARMONICS_SAMPLE_ARRAY {
		SA_NONE			= 0, 
		SA_RAY_ARRAY	= 1, 
		SA_THETA_ARRAY	= 2, 
		SA_PHI_ARRAY	= 4, 
		SA_WEIGHT_ARRAY	= 8,
		SA_SH_ARRAY		= 16
	};

private:
	SphericalHarmonics<order> *mSphericalHarmonicsArray;
};