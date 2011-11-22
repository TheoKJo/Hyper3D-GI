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
 * \author ����
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

	//! dead �� ��� Ư�� ������ ���� �ִٰ� ����.
	void SetDead() { mLivingPhoton = false; }
	//! invalid �� ��� ���⸸ �����ϰ�, ��ġ�� ��ȿ���� ����
	void SetInvalid() { SetDead(); mValidPhoton = false; }

	//! ���������� ���� �ִ� ������ ����
	//! �������� �����ִٰ� �������� ��(Dead Photon) Direction�� ��ġ(Position)�� ������ ���� ������ ����.
	//! ��� �ִ� ������ ���(Living Photon) Direction�� ��ġ(Position)���� ������ ������ ����.
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