/*!
 * \file		Photon.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:14
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

namespace GIEngine { namespace PhotonMapping {
/*!
 * \class Photon
 * \brief ColorPhoton
 * \author 김혁
 * \date 2011.08.22
 */
class GIPhoton
{
public:
	GIPhoton() : mLivingProbability(0.1f), mLivingPhoton(true), mValidPhoton(true), mLastPass(0) {}

	GIVector3 Color;
	//float mIntensity;
	//polarization

	GIVector3 Position;
	GIVector3 Direction;

#ifdef _DEBUG
	unsigned int mLastPass;
#endif

	void Initialize()
	{
		mLivingPhoton = true;
		mValidPhoton = true;
		mLastPass = 0;
	}

	//! dead 일 경우 특정 지역에 멈춰 있다고 가정.
	void SetDead() { mLivingPhoton = false; }
	//! invalid 일 경우 방향만 유요하고, 위치는 유효하지 않음
	void SetInvalid() { SetDead(); mValidPhoton = false; }

	//! 물리적으로 멈춰 있는 포톤은 없음
	//! 가상으로 멈춰있다고 가정했을 때(Dead Photon) Direction은 위치(Position)로 포톤이 들어온 방향을 뜻함.
	//! 살아 있는 포톤의 경우(Living Photon) Direction은 위치(Position)에서 나가는 방향을 뜻함.
	bool IsLivingPhoton() const { return mLivingPhoton; }
	bool IsValidPhoton() const { return mValidPhoton; }

	void SetLivingProbability( float LivingProbability ) { mLivingProbability = LivingProbability; }
	float GetLivingProbability() const { return mLivingProbability; }

private:
	float mLivingProbability;
	bool mLivingPhoton;
	bool mValidPhoton;
};

};};