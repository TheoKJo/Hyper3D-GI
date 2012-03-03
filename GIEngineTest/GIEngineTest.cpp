#include "GIEngineTestPCH.h"
#include "RaytracerTest.h"

#include "../IrrlichtWrapper/GIEngineIrrlicht.h"

#include "GIEngine.h"
#include "Camera.h"

#include "gtest/gtest.h"

// ªË¡¶
#include "Raytracer/SceneAccelStructure.h"
#include "Raytracer/KDTreeStructure.h"

#include <tchar.h>
#include <conio.h>

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

using namespace GIEngine;
using namespace irr;

GIEngineIrrlicht::IrrlichtGISceneManager *g_SceneManager = NULL;

GIEngine::GIScene* g_TestScene;
GIEngine::SceneAccelStructure* g_AccelStruecture;
GIEngine::GIRayCamera* g_Camera;

bool InitlaizeScenes()
{
// Irrlicht Engine
	IrrlichtDevice* device = createDevice( video::EDT_DIRECT3D9, core::dimension2d<u32>(640, 480));

	if (device == 0)
		return false;

	//device->setWindowCaption( "Load .irr file example" );

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	smgr->loadScene("example.irr");
	scene::ICameraSceneNode * camera = smgr->addCameraSceneNodeFPS(0, 50.f, 0.1f);

	g_TestScene = g_SceneManager->ConvertScene( smgr->getRootSceneNode() );
	g_Camera = (GIRayCamera*)g_SceneManager->ConvertCamera( camera, 80, 60 );

	g_AccelStruecture = GIEngine::BuildAccStructure( g_TestScene, NULL );

	g_AccelStruecture->GetKDTree()->SaveToFile( "kdtree.txt" );

// End of Irrlicht Engine

	/*for( unsigned int i = 0; i < MAX_SCENES; i++ )
	{
		g_TestScenes[i] = new GIEngine::GIScene;
		g_Cameras[i] = new GIEngine::GIRayCamera;
	}*/

	return true;
}

void FinalizeScenes()
{
	for( unsigned int i = 0; i < MAX_SCENES; i++ )
	{
		delete g_TestScene;
		delete g_Camera;
	}
}

int main( int argc, char** argv )
//int _tmain( int argc, _TCHAR* argv[] )
{ 
	if( !InitlaizeScenes() )
	{
		return -1;
	}
	// TODO: change camera rays to spherical ray sampling
	InitializeRaytracerTest( g_TestScene, g_AccelStruecture, g_Camera->GetCameraRays(), g_Camera->GetCameraRayCount() );

	testing::InitGoogleTest( &argc, argv ); 
	RUN_ALL_TESTS();

	FinalizeRaytracerTest();
	FinalizeScenes();

	getch();
	return 0;
}