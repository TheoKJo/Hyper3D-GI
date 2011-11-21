#pragma once

#include "RtScene.h"
#include "Camera.h"
#include "Structures.h"
#include "Utility.h"
#include "../GIEngine/SampleArray.h"

#include "KDTreeStructure.h"

//typedef void (*HitCollbackFunction)( const GIRay &Ray, const &GIHit Hit, GIVector4 *outColor );
//typedef void (*HitCollbackFunction)( const GIRay&, const &GIHit, GIVector4* );

/*! \namespace Raytracer
 * \brief 
 * �� �Ծ��� ��Ű�� ���� Ŭ������, ���� ���� ������ �ٸ� �Ծ�� ������ ������ ���´�.
 * �� Ŭ������ ���߱� ���� Raytracer ��� Gamebryo Wrapper Ŭ������ ������.
 * \author ����
 * \date  2010.10.18 ~
 */
namespace GIEngine { namespace Raytracer
{
// Initializations
	//void SetScene( RtScene *pScene ) { rtScene = pScene; }
	//! Ray Array is width first (left to right)

	struct RaytracerOption
	{
		bool PassThroughBackHit;
		bool PassThroughSelfHit;
		float RayEpsilon;
	};

	bool BuildStructure( RtScene *pScene, const char *strStructureFilename, bool bLoadKDTree = true );

// Rendering
	//void RenderToMemory( RayArray, RtAccelStruct );
	//void GenerateRayPackets2x2( GICamera *pCamera, GIRay *RayArray );
	//void GenerateRayPackets4x4( GICamera *pCamera, GIRay *RayArray );
	
	//void InitializeHemisphericalRays( unsigned int RayCount, GIVector3 Position, GIVector3 Normal, DirectionalRaySample *DirectionalRaySampleArray );

	//! With Acceleration Structure
	GIHit ShootRay( RtScene *rtScene, const GIRay &Ray/*, const RaytracerOption &Option*/ );
	//, KDTreeStructure *pKDTree
	////! Without Acceleration Structure (eg. KD-Tree)
	//GIHit ShootRay( RtScene *rtScene, const GIRay &Ray );

	// �Ⱦ���
	void TraverseKDTree( RtScene *rtScene, const GIRay &Ray, RtKDTreeNode *pNode, GIHit &Hit );

	void Shading( RtScene *rtScene, const GIRay &Ray, const GIHit &Hit, int MaxDepth, GIVector4 *outColor );
	//void Shading( GIVector4 *outColor, const GIRay &Ray, const GIHit &Hit );

// Implementations
	void Intersection( RtScene *rtScene, unsigned int *TriangleIndexArray, const int &TriangleCount, const GIRay &Ray, GIHit &Hit );
	void Intersection( const RtTriAccel *TriAccelArray, const int &TriangleCount, const GIRay &Ray, GIHit &Hit );

// Implementations
	void Intersection( const GITriangle &Triangle, const GIRay &Ray, GIHit &Hit );
	void Intersection( const RtTriAccel &TriAccel, const GIRay &Ray, GIHit &Hit );

// Global Setting
	//RtScene *rtScene;
	//GIVector4 BackgroundColor;
};};