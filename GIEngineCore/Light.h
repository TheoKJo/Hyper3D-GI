#pragma once

#include "Global.h"

class GILight
{
public:
	GIColor4 AmbientColor;
	GIColor4 DiffuseColor;
	GIColor4 SpecularColor;
	GIColor4 EmissionColor;

	virtual bool HasAttenuation() = 0;
	// Ư�� ���⿡ ���ؼ��� ��������� ��Ÿ��. Area Light�� �ݴ�Ǵ� ������(PBRT ����).
	virtual bool IsDiracDeltaLight() = 0;
};

class GIPointLight : public GILight
{
public:
	GIPointLight() : ConstantAttenuation(1.0f), LinearAttenuation(0.0f), QuadraticAttenuation(0.0f) {}
	bool HasAttenuation() { return true; }
	bool IsDiracDeltaLight() { return true; }

	GIVector3 Position;

	float ConstantAttenuation;
	float LinearAttenuation;
	float QuadraticAttenuation;
};

//! from infinite distance
class GIDirectionalLight : public GILight
{
public:
	GIVector3 Direction;
	bool HasAttenuation() { return false; }
	bool IsDiracDeltaLight() { return true; }
};