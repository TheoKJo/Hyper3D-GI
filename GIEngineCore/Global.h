/*!
 * \file		Global.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:17
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

// TODO: 삭제
#pragma warning(disable : 4201)

#ifndef GI_ENGINE_GLOBAL
#define GI_ENGINE_GLOBAL

#ifndef NULL
#define NULL 0
#endif

#define FLOAT_EPSILON 0.001f
#define FLOAT_PI 3.141592f
#define GI_INFINITY 1e15f

#include <math.h>

template<class T>
void SafeDelete( T **obj )
{
	if( obj != NULL )
	{
		if( *obj != NULL )
		{
			delete *obj;
			*obj = NULL;
		}
	}
}

template<class T>
void SafeDeleteArray( T **obj )
{
	if( obj != NULL )
	{
		if( *obj != NULL )
		{
			delete[] *obj;
			*obj = NULL;
		}
	}
}

struct GIVector2
{
	union {
		float array[2];
		// struct { float x, y, z; }
		struct { float x, y; };
		struct { float u, v; };
	};

	GIVector2() {}

	GIVector2( const float &_x, const float &_y )
	{
		array[0] = _x;
		array[1] = _y;
	}
	GIVector2( const float *FloatArray )
	{
		array[0] = FloatArray[0];
		array[1] = FloatArray[1];
	}

	//const float& x( const float& _x ) { return array[0] = _x; }
	//const float& y( const float& _y ) { return array[1] = _y; }

	//const float& x() const { return array[0]; }
	//const float& y() const { return array[1]; }

	//const float& u( const float& u ) { return array[0] = u; }
	//const float& v( const float& v ) { return array[1] = v; }

	//const float& u() const { return array[0]; }
	//const float& v() const { return array[1]; }

	//const float& s( const float& s ) { return array[0] = s; }
	//const float& t( const float& t ) { return array[1] = t; }	

	//const float& s() const { return array[0]; }
	//const float& t() const { return array[1]; }

	inline GIVector2 operator-() const
	{
		return GIVector2( -y, -y );
	}

	inline GIVector2 operator+( const GIVector2 &vec ) const
	{
		return GIVector2( x + vec.x, y + vec.y );
	}
	inline GIVector2 operator-( const GIVector2 &vec ) const
	{
		return GIVector2( x - vec.x, y - vec.y );
	}
	inline GIVector2 operator*( const float scalar ) const
	{
		return GIVector2( x * scalar, y * scalar );
	}
	inline GIVector2 operator/( const float scalar ) const
	{
		return GIVector2( x / scalar, y / scalar );
	}
};

struct GIVector3
{
	union {
		float array[3];
		struct { float x, y, z; };
		struct { float r, g, b; };
		//struct { float u, v, q; };
	};

	GIVector3() {}
	GIVector3( const float _x, const float _y, const float _z )
		: x(_x), y(_y), z(_z)
	{}
	GIVector3( const float *FloatArray )
		: x(FloatArray[0]), y(FloatArray[1]), z(FloatArray[2])
	{}

	inline GIVector3 GetNormalized() const
	{
		float Length = GetLength();
		return GIVector3( x/Length, y/Length, z/Length );
	}

	inline float GetLengthSquared() const
	{
		return x * x + y * y + z * z;
	}

	inline float GetLength() const
	{
		return sqrt( GetLengthSquared() );
	}

	inline GIVector3 operator-() const
	{
		return GIVector3( -x, -y, -z );
	}

	inline GIVector3 operator+( const GIVector3 &vec ) const
	{
		return GIVector3( x + vec.x, y + vec.y, z + vec.z );
	}
	inline GIVector3 operator-( const GIVector3 &vec ) const
	{
		return GIVector3( x - vec.x, y - vec.y, z - vec.z );
	}
	inline GIVector3 operator*( const GIVector3 &vec ) const
	{
		return GIVector3( x * vec.x, y * vec.y, z * vec.z );
	}
	inline GIVector3 operator*( const float scalar ) const
	{
		return GIVector3( x * scalar, y * scalar, z * scalar );
	}
	inline GIVector3 operator*=( const float scalar )
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	inline GIVector3 operator/( const float scalar ) const
	{
		return GIVector3( x / scalar, y / scalar, z / scalar );
	}
	inline GIVector3 operator/=( const float scalar )
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	inline GIVector3 operator+=( const GIVector3 &vec )
	{
		x += vec.x; y += vec.y; z += vec.z;
		return *this;
	}

	inline GIVector3 operator=( const GIVector3 &vec )
	{
		x = vec.x; y = vec.y; z = vec.z;
		return *this;
	}

	inline float DotProduct( const GIVector3 &vec ) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	};
	inline GIVector3 CrossProduct( const GIVector3 &vec ) const
	{
		return GIVector3( 
			y * vec.z - z * vec.y, 
			z * vec.x - x * vec.z, 
			x * vec.y - y * vec.x
			);
	};

	inline void CopyToFloatArray( float *FloatArray ) const
	{
		FloatArray[0] = x;
		FloatArray[1] = y;
		FloatArray[2] = z;
	}
};

struct GIVector4
{
	union {
		float array[4];
		struct { float x, y, z, w; };
		//struct { float u, v, q, r; };
		//struct { float s, t, q, r; };
	};

	GIVector4() {}
	GIVector4( const float _x, const float _y, const float _z, const float _w = 1.0f )
		: x(_x), y(_y), z(_z), w(_w)
	{}
	GIVector4( const GIVector3 &vec3 )
		: x(vec3.x), y(vec3.y), z(vec3.z), w(1.0f)
	{}
	GIVector4( const float *FloatArray )
		: x(FloatArray[0]), y(FloatArray[1]), z(FloatArray[2]), w(FloatArray[3])
	{}

	inline GIVector3 ToVector3()
	{
		if( w == 0.0f )
			return GIVector3( x, y, z);
		return GIVector3( x/w, y/w, z/w);
	}

	inline GIVector4 operator-() const
	{
		//return GIVector4( -x, -y, -z, w );
		return GIVector4( -x, -y, -z, -w );//??
	}

	inline GIVector4 operator+( const GIVector4 &vec ) const
	{
		return GIVector4( x + vec.x, y + vec.y, z + vec.z, w + vec.w );
	}
	inline GIVector4 operator-( const GIVector4 &vec ) const
	{
		return GIVector4( x - vec.x, y - vec.y, z - vec.z, w - vec.w );
	}
	inline GIVector4 operator*( const GIVector4 &vec ) const
	{
		return GIVector4( x * vec.x, y * vec.y, z * vec.z, w * vec.w );
	}
	inline GIVector4 operator*( const float scalar ) const
	{
		return GIVector4( x * scalar, y * scalar, z * scalar, w * scalar );
	}
	inline GIVector4 operator/( const float scalar ) const
	{
		return GIVector4( x / scalar, y / scalar, z / scalar, w / scalar );
	}

	inline GIVector4& operator+=( const GIVector4 &vec )
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
		return *this;
	}


	inline GIVector4 operator=( const GIVector4 &vec )
	{
		x = vec.x; y = vec.y; z = vec.z; w = vec.w;
		return *this;
	}

	GIVector4 ComponentWiseProduct( const GIVector4 &vec )
	{
		return GIVector4( x * vec.x, y * vec.y, z * vec.z, w * vec.w );
	}

	inline void CopyToFloatArray( float *FloatArray ) const
	{
		FloatArray[0] = x;
		FloatArray[1] = y;
		FloatArray[2] = z;
		FloatArray[3] = w;
	}	
};

typedef GIVector3 GIColor3;
typedef GIVector4 GIColor4;

//! Axis Aligned Bounding Box
class GIBoundingBox
{
public:
	GIVector3 MinPositions;
	GIVector3 MaxPositions;

	GIVector3 GetDiagonal() const
	{
		return MaxPositions - MinPositions;
	}

	int GetLongestAxis() const
	{
		int Axis = -1;
		const GIVector3 distancePosition = GetDiagonal();
		if( distancePosition.array[0] < distancePosition.array[1] )
		{
			if( distancePosition.array[1] < distancePosition.array[2] )
				Axis = 2;
			else
				Axis = 1;
		}
		else
		{
			if( distancePosition.array[0] < distancePosition.array[2] )
				Axis = 2;
			else
				Axis = 0;
		}
		return Axis;
	}
};

// TODO: 정말 안좋은 방식. 안쓰이는 것까지 모두 들어감.
struct GIVertexGroup
{
	GIVertexGroup() :
		VertexNum(-1), 
		Vertex( 0.0f, 0.0f, 0.0f ), 
		Normal( 0.0f, 0.0f, 1.0f ), 
		Tangent( 1.0f, 0.0f, 0.0f ), 
		Color( 0.0f, 0.0f, 0.0f, 1.0f ), 
		TexCoords( 0.0f, 0.0f )
	{}

	int VertexNum;
	GIVector3 Vertex;
	GIVector3 Normal;
	GIVector3 Tangent;
	GIVector4 Color;
	GIVector2 TexCoords;
};

struct GITriangle
{
	GITriangle() : TriangleNumber(-1), MaterialIndex(-1), mFlag(0) {}
	//union {
	//	//GIVertexGroup array[3];
	//	//struct { GIVertexGroup vg0, vg1, vg2; };
	//	
	//};

	GIVertexGroup vg0;
	GIVertexGroup vg1;
	GIVertexGroup vg2;

	// Barycentric
	int TriangleNumber;
	inline bool HasMaterial() const { return MaterialIndex != -1; }
	int MaterialIndex;

	inline void CalcVertexTangents()
	{
		const GIVector2 &TexCoords0 = vg0.TexCoords;
		const GIVector2 &TexCoords1 = vg1.TexCoords;
		const GIVector2 &TexCoords2 = vg2.TexCoords;

		// TODO: u2가 안쓰임. 제대로 이해했는지 확인 할 것.
		// vertex0 to vertex1
		float u1 = TexCoords1.u - TexCoords0.u;
		float v1 = TexCoords1.v - TexCoords0.v;

		// vertex0 to vertex2
		float u2 = TexCoords2.u - TexCoords0.u;
		float v2 = TexCoords2.v - TexCoords0.v;

		float det = u1*v2 - v2*u1;

		const GIVector3 Vector1 = vg1.Vertex - vg0.Vertex;
		const GIVector3 Vector2 = vg2.Vertex - vg0.Vertex;

		GIVector3 Tangent = Vector1 * v2/det + Vector2 * -v1/det;
		//GIVector3 Binormal = Vector1 * -u2/det + Vector2 * u1/det;

		GIVector3 BinormalTemp;
		BinormalTemp = vg0.Normal.CrossProduct( Tangent );
		vg0.Tangent = BinormalTemp.CrossProduct( vg0.Normal );	

		BinormalTemp = vg1.Normal.CrossProduct( Tangent );
		vg1.Tangent = BinormalTemp.CrossProduct( vg1.Normal );

		BinormalTemp = vg2.Normal.CrossProduct( Tangent );
		vg2.Tangent = BinormalTemp.CrossProduct( vg2.Normal );
	}

	//SetFlag(
	//HasNormal
	//IsGlobalNormal?? Unity??
	//HasTangent

	void SetHasTexCoords() { mFlag = mFlag | ETF_TEXCOORDS; }
	void SetHasNormal() { mFlag = mFlag | ETF_NORMAL; }
	void SetHasTangent() { mFlag = mFlag | ETF_TANGENT; }

	bool HasTexCoords() { return (mFlag & ETF_TEXCOORDS) > 0; }
	bool HasNormal() { return (mFlag & ETF_NORMAL) > 0; }
	bool HasTangent() { return (mFlag & ETF_TANGENT) > 0; }

private:
	enum ETriangleFlag {
		ETF_TEXCOORDS = 1, 
		ETF_NORMAL = 2, 
		ETF_TANGENT = 4
		//GITF_MATERIAL, 
	};
	unsigned int mFlag;
};

struct GIQuadrangle
{
	GIQuadrangle() : QuadrangleNumber(-1), MaterialIndex(-1), mFlag(0) {}

	int QuadrangleNumber;
	int MaterialIndex;

	GIVertexGroup vg0;
	GIVertexGroup vg1;
	GIVertexGroup vg2;
	GIVertexGroup vg3;

	GIVector3 CenterPosition;
	GIVector3 CenterNormal;

private:
	unsigned int mFlag;
};

struct GIRay
{
	float org[3];
	float dir[3];
	float NearDistance;
	float FarDistance;
};


//! Acceleration Structure (KD-Tree)
class RtAccelStructure
{
};

struct RtTriAccel
{
	float n_u;	//!< normal.u/normal.k
	float n_v;	//!< normal.v/normal.k
	float n_d;	//!< constant of plane equation (n dot v0)/n.k
	int k;		//!< projection dimension

	float b_nu;
	float b_nv;
	float b_d;
	unsigned int triNum;

	float c_nu;
	float c_nv;
	float c_d;
	int pad;
};

struct GIHit
{
	GIHit() : triNum(-1), dist(1e16f), hit(false), backHit(false) {}
	//GIHit( const GIRay &_ray ) : triNum(-1), dist(1e16f), hit(false), backHit(false), ray(_ray) {}

	void Reset()
	{
		triNum = -1;
		dist = 1e16f;
		hit = false;
	}

	bool isHit() const { return hit; }

	bool hit;
	float dist;
	int triNum;
	float u;
	float v;
	bool backHit;
	//GIRay ray;
};


#endif