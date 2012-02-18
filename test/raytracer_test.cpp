/*!
 * \file		raytracer_test.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/02/14
 * created:		2012/02/14 0:05
 * copyright	Hybrid (creavol@gmail.com)
 */

#include "GIEngine.h"

#include "gtest/gtest.h"

#include <math.h>

const float DISTANCE_EPSILON = 0.001f;
const float UV_EPSILON = 0.001f;

//int test( int x, int y )
//{
//	
//	ASSERT_EQ(x.size(), y.size()) << "Vectors x and y are of unequal length";
//
//	for (int i = 0; i < x.size(); ++i) {
//		EXPECT_EQ(x[i], y[i]) << "Vectors x and y differ at index " << i;
//	}
//}


// TODO: absolute 와 relative를 다른 함수로?
bool almost_equal( float a, float b, float epsilon )
{
	// relative error
	if( a == b )
		return true;
	// TODO: b == 0, then absolute error check
	return fabs((a-b) / b) < epsilon;
	
	// absolute error
	//return fabs( a - b ) < epsilon;
}

bool equal( const GIHit &a, const GIHit &b )
{
	if( a.backHit != b.backHit )
		return false;
	if( a.hit != b.hit )
		return false;
	if( a.triNum != b.triNum )
		return false;
	
	if( !almost_equal( a.dist, b.dist, DISTANCE_EPSILON ) )
		return false;
	if( !almost_equal( a.u, b.u, UV_EPSILON ) )
		return false;
	if( !almost_equal( a.v, b.v, UV_EPSILON ) )
		return false;

	return true;
}

/*!
 * Raytracer Tests.
 * Assume that cpu raytracer without kdtree generates valid result.
 */
TEST( RaytracerTestCase, Scene1Test )
{
	// for scene 1 and some ray arrays

	// for a ray
	GIHit cpu_linear_result = GIHit();
	GIHit cpu_kdtree_result = GIHit();
	GIHit gpu_kdtree_result = GIHit();

	EXPECT_TRUE( equal( cpu_linear_result, cpu_kdtree_result ) );
	EXPECT_TRUE( equal( cpu_linear_result, gpu_kdtree_result ) );
}

TEST( RaytracerTestCase, Scene2Test )
{
	// for scene 1 and some ray arrays

	// for a ray
	GIHit cpu_linear_result = GIHit();
	GIHit cpu_kdtree_result = GIHit();
	GIHit gpu_kdtree_result = GIHit();

	EXPECT_TRUE( equal( cpu_linear_result, cpu_kdtree_result ) );
	EXPECT_TRUE( equal( cpu_linear_result, gpu_kdtree_result ) );
}

TEST( RaytracerTestCase, Scene3Test )
{
	// for scene 1 and some ray arrays

	// for a ray
	GIHit cpu_linear_result = GIHit();
	GIHit cpu_kdtree_result = GIHit();
	GIHit gpu_kdtree_result = GIHit();

	EXPECT_TRUE( equal( cpu_linear_result, cpu_kdtree_result ) );
	EXPECT_TRUE( equal( cpu_linear_result, gpu_kdtree_result ) );
}

/*!
 * Spherical Harmonics Tests
 */
TEST( SphericalHarmonicsTestCase, ProjectionTest )
{
	EXPECT_TRUE( true );
}

TEST( SphericalHarmonicsTestCase, 1 )
{
	EXPECT_TRUE( true );
}
