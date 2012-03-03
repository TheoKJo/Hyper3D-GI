/*!
 * \file		Raytracer.h
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2011/11/22
 * created:		2011/11/22 21:15
 * copyright	Hybrid (creavol@gmail.com)
 */
#pragma once

//#include <Scene.h>
//#include <Camera.h>
//#include "Structures.h"
// core
#include <Utility.h>
#include "../SampleArray.h"

//#include "KDTreeStructure.h"

//typedef void (*HitCollbackFunction)( const GIRay &Ray, const &GIHit Hit, GIVector4 *outColor );
//typedef void (*HitCollbackFunction)( const GIRay&, const &GIHit, GIVector4* );

/*! 
 * \namespace Raytracer
 * \brief 
 * \author Hybrid
 * \date  2010.10.18 ~
 */
namespace GIEngine { 
	class GIScene;
	class Camera;
	class GIKDTreeNode;
	class KDTreeStructure;

	namespace Raytracer {
// Initializations
	//void SetScene( GIScene *pScene ) { rtScene = pScene; }
	//! Ray Array is width first (left to right)

	KDTreeStructure* BuildKDTree( GIScene *pScene, const char *strStructureFilename, bool bLoadKDTree = true );

// Rendering
	//void RenderToMemory( RayArray, RtAccelStruct );
	//void GenerateRayPackets2x2( GICamera *pCamera, GIRay *RayArray );
	//void GenerateRayPackets4x4( GICamera *pCamera, GIRay *RayArray );
	
	//void InitializeHemisphericalRays( unsigned int RayCount, GIVector3 Position, GIVector3 Normal, DirectionalRaySample *DirectionalRaySampleArray );

	//! With Acceleration Structure
	GIHit ShootRay( GIScene *rtScene, KDTreeStructure *KDTree, const GIRay &Ray/*, const RaytracerOption &Option*/ );
	//, KDTreeStructure *pKDTree
	////! Without Acceleration Structure (eg. KD-Tree)
	GIHit ShootRay( GIScene *rtScene, const GIRay &Ray );

	// ╬х╬╡юс
	void TraverseKDTree( GIScene *rtScene, const GIRay &Ray, GIKDTreeNode *pNode, GIHit &Hit );

	void Shading( GIScene *rtScene, KDTreeStructure *KDTree, const GIRay &Ray, const GIHit &Hit, int MaxDepth, GIVector4 *outColor );
	//void Shading( GIVector4 *outColor, const GIRay &Ray, const GIHit &Hit );

// Implementations
	void Intersection( GIScene *rtScene, unsigned int *TriangleIndexArray, const int &TriangleCount, const GIRay &Ray, GIHit &Hit );
	void Intersection( const GITriAccel *TriAccelArray, const int &TriangleCount, const GIRay &Ray, GIHit &Hit );

// Implementations
	void Intersection( const GITriangle &Triangle, const GIRay &Ray, GIHit &Hit );
	void Intersection( const GITriAccel &TriAccel, const GIRay &Ray, GIHit &Hit );

// Global Setting
	//GIScene *rtScene;
	//GIVector4 BackgroundColor;
};};