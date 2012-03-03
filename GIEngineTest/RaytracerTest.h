#define MAX_SCENES 1

#include <GIEngine.h>
//#include <Raytracer/Raytracer.h>

namespace GIEngine {
	class GIScene;
	class SceneAccelStructure;
	class GIRayCamera;
};

#define TEST_RUN_NAME_MAX 512

struct RaytracerTestRun
{
	char TestRunName[TEST_RUN_NAME_MAX];

	GIEngine::RaytracerOption Option;

	GIEngine::GIScene *Scene;
	GIEngine::SceneAccelStructure *AccelStruecture;

	unsigned int RayCount;
	GIRay *RayArray;
	GIHit *HitResult;

	bool Enabled;
};

#define DISTANCE_EPSILON 0.001f
#define UV_EPSILON 0.001f

// TODO: absolute 와 relative를 다른 함수로?
inline bool almost_equal( float a, float b, float epsilon )
{
	// relative error
	if( a == b )
		return true;
	// TODO: b == 0, then absolute error check
	return fabs((a-b) / b) < epsilon;
	
	// absolute error
	//return fabs( a - b ) < epsilon;
}

inline bool equal( const GIHit &a, const GIHit &b )
{
	if( a.hit != b.hit )
		return false;

	if( a.hit )
	{
		if( !almost_equal( a.dist, b.dist, DISTANCE_EPSILON ) )
			return false;

		return true;
		// might be approximately correct when checks below this line are failed
		if( a.triNum != b.triNum )
			return false;
		if( !almost_equal( a.u, b.u, UV_EPSILON ) )
			return false;
		if( !almost_equal( a.v, b.v, UV_EPSILON ) )
			return false;
		/*if( a.backHit != b.backHit )
			return false;*/
		
	}
	return true;
}

bool InitializeRaytracerTest( GIEngine::GIScene *Scene, GIEngine::SceneAccelStructure *AccelStructure, GIRay *RayArray, unsigned int RayCount );
void FinalizeRaytracerTest();