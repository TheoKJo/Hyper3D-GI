/*!
 * License
 */
#pragma once

#include "GIEngine.h"

#include <NiNode.h>
#include <NiCamera.h>
#include <NiMesh.h>
#include <NiColor.h>

#include <list>

unsigned int CountVerticesRecursive( NiObject *pObject );
unsigned int CountTrianglesRecursive( NiObject *pObject );
unsigned int CountLightRecursive( NiNode *pNode );
unsigned int RefineTriangleRecursive( NiNode *pNode, RtTriangle *outTriangleList, const NiTransform &ParentTransform );

inline RtVector4 ConvertColor( const NiColor &Color, float alpha = 1.0f )
{
	return RtVector4( Color.r, Color.g, Color.b, alpha );
}

inline RtVector3 ConvertVector( const NiPoint3 &Vector )
{
	return RtVector3( Vector.x, Vector.y, Vector.z );
}

inline NiPoint3 ConvertVector( const RtVector3 &Vector )
{
	return NiPoint3( Vector.x, Vector.y, Vector.z );
}

template<class T>
class IrradianceVolume;
class AmbientCube;

/*! \class GamebryoGI
 * \brief Gamebryo ���� ����ϱ� ���� Wrapper Class
 * ���� Raytracer�� ������ Raytracer �� �ְ�, Gamebryo �ڵ带 ������� ����.
 * \author Hybrid
 * \date  2010.10.18 ~
 */
namespace GIEngine
{
class GamebryoGISceneManager : public GISceneManager
{
public:
	GamebryoGISceneManager();
	virtual ~GamebryoGISceneManager();

	GIScene* ConvertScene( NiNode *pkScene );
	RtCamera* ConvertCamera( NiCamera *pkCamera, unsigned int Width, unsigned int Height );

private:
	// ������� ����
	GIScene* CreateScene() { return new GIScene; }

private:
	//! ConvertScene ���ο� �Լ�
	void RefineTriangles( NiNode *pkScene, GIScene *pGIScene );
		unsigned int RefineTrianglesRecursive( GIScene *Scene, NiNode *pNode, unsigned int &VertexIndex, RtTriangle *outTriangleList, std::list<RtMaterial*> &MaterialList );
	void RefineLights( NiNode *pkScene, GIScene *pGIScene );
		unsigned int RefineLightsRecursive( NiNode *pNode, std::list<RtLight*> &LightList );
	void RefineMaterials( NiNode *pkScene, GIScene *pGIScene );

private:
	void InitializeLights();
	void InitializeMaterials();

// ���ο� �Լ�
	int GetMaterialIndex( NiMaterialProperty *pMaterialProperty, NiTexturingProperty *pTextureProperty );
	int GetMaterialIndex( NiMaterial *pMaterial );
	RtTexture* FetchTextureData( NiTexture *inTexture );

public:
	RtLight* ConvertLight( NiLight *pkLight );
	RtMaterial* ConvertMaterial( NiMesh *pkMesh );
	unsigned int MeshToTriangleList( GIScene *Scene, NiMesh *pkMesh, unsigned int &VertexIndex, RtTriangle *outTriangleList, std::list<RtMaterial*> &MaterialList );

	unsigned int RefineLightRecursive( NiNode *pNode, RtLight **outLightArray, const NiTransform &ParentTransform );
};
}