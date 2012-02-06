/*!
 * \file		Light.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:17
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

#include "Global.h"
#include "BinaryDataStream.h"

#include <memory.h>

namespace GIEngine {
class GILight : public BinaryDataStreamRequired
{
public:
	GIColor4 AmbientColor;
	GIColor4 DiffuseColor;
	GIColor4 SpecularColor;
	//GIColor4 EmissionColor;

	virtual bool HasAttenuation() = 0;
	// 특정 방향에 대해서만 계산할지를 나타냄. Area Light와 반대되는 개념임(PBRT 참조).
	virtual bool IsDiracDeltaLight() = 0;

	virtual bool FromBinaryDataStream( const BinaryDataStream &in )
	{
		unsigned int size = sizeof(GIColor4) * 3;
		if( in.GetBinaryDataSize() != size )
			return false;
		unsigned int pos = 0;
		const char* BinaryData = in.GetBinaryData();
		AmbientColor = *((GIColor4*)&BinaryData[pos]);
		pos += sizeof(GIColor4);
		DiffuseColor = *((GIColor4*)&BinaryData[pos]);
		pos += sizeof(GIColor4);
		SpecularColor = *((GIColor4*)&BinaryData[pos]);
		pos += sizeof(GIColor4);
		//EmissionColor = *((GIColor4*)&BinaryData[pos]);
		//pos += sizeof(GIColor4);
		return true;
	}
	virtual unsigned int ToBinaryDataStream( BinaryDataStream *out )
	{
		unsigned int size = sizeof(GIColor4) * 3;
		out->AllocateData( size );
		const char* BinaryData = out->GetBinaryData();
		unsigned int pos = 0;
		memcpy( (void*)&BinaryData[pos], &AmbientColor, sizeof(GIColor4) );
		pos += sizeof(GIColor4);
		memcpy( (void*)&BinaryData[pos], &DiffuseColor, sizeof(GIColor4) );
		pos += sizeof(GIColor4);
		memcpy( (void*)&BinaryData[pos], &SpecularColor, sizeof(GIColor4) );
		pos += sizeof(GIColor4);
		//memcpy( BinaryData[pos], &EmissionColor, sizeof(GIColor4) );
		//pos += sizeof(GIColor4);
		return out->GetBinaryDataSize();
	}
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

	virtual bool FromBinaryDataStream( const BinaryDataStream &in )
	{
		unsigned int size = sizeof(unsigned int) + sizeof(GIVector3) + sizeof(float) * 3;
		if( in.GetBinaryDataSize() != size )
			return false;
		unsigned int pos = 0;
		const char* BinaryData = in.GetBinaryData();
		Position = *((GIVector3*)&BinaryData[pos]);
		pos += sizeof(GIVector3);
		ConstantAttenuation = *((float*)&BinaryData[pos]);
		pos += sizeof(float);
		LinearAttenuation = *((float*)&BinaryData[pos]);
		pos += sizeof(float);
		QuadraticAttenuation = *((float*)&BinaryData[pos]);
		return true;
	}
	virtual unsigned int ToBinaryDataStream( BinaryDataStream *out )
	{
		__super::ToBinaryDataStream( out );

		BinaryDataStream bds;
		unsigned int size = sizeof(unsigned int) + sizeof(GIVector3) + sizeof(float) * 3;
		bds.AllocateData( size );
		const char* BinaryData = bds.GetBinaryData();
		unsigned int pos = 0;
		memcpy( (void*)&BinaryData[pos], &Position, sizeof(GIVector3) );
		pos += sizeof(GIVector3);
		memcpy( (void*)&BinaryData[pos], &ConstantAttenuation, sizeof(float) );
		pos += sizeof(float);
		memcpy( (void*)&BinaryData[pos], &LinearAttenuation, sizeof(float) );
		pos += sizeof(float);
		memcpy( (void*)&BinaryData[pos], &QuadraticAttenuation, sizeof(float) );

		return out->AddBinaryDataStream( bds );
	}
};

//! from infinite distance
class GIDirectionalLight : public GILight
{
public:
	GIVector3 Direction;
	bool HasAttenuation() { return false; }
	bool IsDiracDeltaLight() { return true; }

	virtual bool FromBinaryDataStream( const BinaryDataStream &in )
	{
		unsigned int size = sizeof(GIVector3) + sizeof(float) * 3;
		if( in.GetBinaryDataSize() != size )
			return false;
		const char* BinaryData = in.GetBinaryData();
		Direction = *((GIVector3*)BinaryData);
		return true;
	}
	virtual unsigned int ToBinaryDataStream( BinaryDataStream *out )
	{
		__super::ToBinaryDataStream( out );

		BinaryDataStream bds;
		unsigned int size = sizeof(GIVector3) + sizeof(float) * 3;
		bds.AllocateData( size );
		char* BinaryData = bds.GetBinaryData();
		memcpy( (void*)BinaryData, &Direction, sizeof(GIVector3) );

		return out->AddBinaryDataStream( bds );		
	}
};
};