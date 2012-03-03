/*!
 * \file		raytracer_test.cpp
 * \brief		
 * \author		Hybrid (creavol@gmail.com)
 * \date		2012/02/14
 * created:		2012/02/14 0:05
 * copyright	Hybrid (creavol@gmail.com)
 */

#include "GIEngineTestPCH.h"
#include "RaytracerTest.h"

#include "GIEngine.h"
#include "../GIEngineCUDA/GIEngineCUDA.h"
#include "Raytracer\Raytracer.h"
#include "Raytracer\SceneAccelStructure.h"
#include "Camera.h"

#include "gtest/gtest.h"

#include <time.h>
#include <math.h>
#include <vector>

RaytracerTestRun TestRuns[RAYTRACER_TEST_RUN_COUNT];

RaytracerTestRun *CPU_LINEAR_1_THREAD_TEST_RUN = &TestRuns[0];

// With KD-Tree
RaytracerTestRun *CPU_KDTREE_1_THREAD_TEST_RUN = &TestRuns[1];
RaytracerTestRun *CPU_KDTREE_2_THREAD_TEST_RUN = &TestRuns[2];
RaytracerTestRun *CPU_KDTREE_4_THREAD_TEST_RUN = &TestRuns[3];
RaytracerTestRun *CPU_KDTREE_5_THREAD_TEST_RUN = &TestRuns[4];
RaytracerTestRun *CPU_KDTREE_8_THREAD_TEST_RUN = &TestRuns[5];
RaytracerTestRun *CPU_KDTREE_16_THREAD_TEST_RUN = &TestRuns[6];

RaytracerTestRun *CUDA_LINEAR_TEST_RUN = &TestRuns[7];
RaytracerTestRun *CUDA_KDTREE_TEST_RUN = &TestRuns[8];

void RunRaytracerTestRun( const RaytracerTestRun &TestRun )
{
	// TODO: Raytracer Option을 안으로.
	if( TestRun.Option.RenderingDeviceType == GIEngine::RaytracerOption::RDT_CPU )
	{
		GIEngine::KDTreeStructure *KDTree = NULL;
		if( TestRun.AccelStruecture != NULL )
			KDTree = TestRun.AccelStruecture->GetKDTree();
		GIEngine::Raytracer::SampleHitCPU( TestRun.Option.NumberOfThreads, 
			TestRun.Scene, KDTree, TestRun.RayArray, TestRun.RayCount, TestRun.HitResult );
	}
	else if( TestRun.Option.RenderingDeviceType == GIEngine::RaytracerOption::RDT_CUDA )
	{
		GIEngine::KDTreeStructureCUDA *KDTree = NULL;
		if( TestRun.AccelStruecture != NULL )
			KDTree = TestRun.AccelStruecture->GetKDTreeCUDA();
		GIEngine::Raytracer::SampleHitCUDA( TestRun.Scene, KDTree, TestRun.RayArray, TestRun.RayCount, TestRun.HitResult );
	}
	else
	{
		assert( false );
	}
}

bool InitializeRaytracerTest( GIEngine::GIScene *Scene, GIEngine::SceneAccelStructure *AccelStructure, GIRay *RayArray, unsigned int RayCount )
{
	for( unsigned int i = 0; i < RAYTRACER_TEST_RUN_COUNT; i++ )
	{
		TestRuns[i].Enabled = false;
		TestRuns[i].Scene = Scene;
		TestRuns[i].AccelStruecture = AccelStructure;
		TestRuns[i].RayArray = RayArray;
		TestRuns[i].RayCount = RayCount;
		TestRuns[i].HitResult = new GIHit[RayCount];
		TestRuns[i].Option.RenderingDeviceType = GIEngine::RaytracerOption::RDT_CPU;
	}

	// TODO: From txt files or script

	// CPU Raytracer, 1 Thread
	strcpy_s( CPU_LINEAR_1_THREAD_TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "CPU Raytracer w/o KDTree, 1 Thread" );
	CPU_LINEAR_1_THREAD_TEST_RUN->Option.NumberOfThreads = 1;
	CPU_LINEAR_1_THREAD_TEST_RUN->AccelStruecture = NULL;
	CPU_LINEAR_1_THREAD_TEST_RUN->Enabled = true;

	// CPU Raytracer, 1 Thread
	strcpy_s( CPU_KDTREE_1_THREAD_TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "CPU Raytracer, 1 Thread" );
	CPU_KDTREE_1_THREAD_TEST_RUN->Option.NumberOfThreads = 1;
	//CPU_KDTREE_1_THREAD_TEST_RUN->Enabled = true;

	// CPU Raytracer, 2 Thread
	strcpy_s( CPU_KDTREE_2_THREAD_TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "CPU Raytracer, 2 Threads" );
	CPU_KDTREE_2_THREAD_TEST_RUN->Option.NumberOfThreads = 2;
	//CPU_KDTREE_2_THREAD_TEST_RUN->Enabled = true;

	// CPU Raytracer, 4 Thread
	strcpy_s( CPU_KDTREE_4_THREAD_TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "CPU Raytracer, 8 Threads" );
	CPU_KDTREE_4_THREAD_TEST_RUN->Option.NumberOfThreads = 4;
	CPU_KDTREE_4_THREAD_TEST_RUN->Enabled = true;

	// CPU Raytracer, 5 Thread
	strcpy_s( CPU_KDTREE_5_THREAD_TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "CPU Raytracer, 5 Threads" );
	CPU_KDTREE_5_THREAD_TEST_RUN->Option.NumberOfThreads = 5;
	//CPU_KDTREE_5_THREAD_TEST_RUN->Enabled = true;

	// CPU Raytracer, 8 Thread
	strcpy_s( CPU_KDTREE_8_THREAD_TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "CPU Raytracer, 8 Threads" );
	CPU_KDTREE_8_THREAD_TEST_RUN->Option.NumberOfThreads = 8;
	//CPU_KDTREE_8_THREAD_TEST_RUN->Enabled = true;

	// CPU Raytracer, 16 Thread
	strcpy_s( CPU_KDTREE_16_THREAD_TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "CPU Raytracer, 16 Threads" );
	CPU_KDTREE_16_THREAD_TEST_RUN->Option.NumberOfThreads = 16;
	//CPU_KDTREE_16_THREAD_TEST_RUN->Enabled = true;

	// CUDA Raytracer with KDTree
	strcpy_s( CUDA_LINEAR_TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "CUDA Raytracer w/o KDTree" );
	CUDA_LINEAR_TEST_RUN->Option.RenderingDeviceType = GIEngine::RaytracerOption::RDT_CUDA;
	CUDA_LINEAR_TEST_RUN->Enabled = false;	

	// CUDA Raytracer with KDTree
	strcpy_s( CUDA_KDTREE_TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "CUDA Raytracer" );
	// TODO: without 'Raytracer:: ?'
	CUDA_KDTREE_TEST_RUN->Option.RenderingDeviceType = GIEngine::RaytracerOption::RDT_CUDA;
	CUDA_KDTREE_TEST_RUN->Enabled = true;

	// OpenCL Raytracer
	/*strcpy_s( OPENCL_LINEAR__TEST_RUN->TestRunName, TEST_RUN_NAME_MAX, "OpenCL Raytracer" );
	OPENCL_LINEAR__TEST_RUN->Option.RenderingDeviceType = Raytracer::RaytracerOption::RDT_OPENCL;*/
	//OPENCL_LINEAR__TEST_RUN.Enabled = true;

	//float test[] = { 24, 23, 12, 12, 11, 11, 15, 0.0f };

	for( unsigned int i = 0; i < RAYTRACER_TEST_RUN_COUNT; i++ )
	{
		if( TestRuns[i].Enabled == false )
			continue;

		printf( "TestRun: %s ", TestRuns[i].TestRunName );
		clock_t before = clock();
		RunRaytracerTestRun( TestRuns[i] );
		double elaspedTime = (double)(clock() - before) / CLOCKS_PER_SEC;
		printf( "(%.0f ms)\n", elaspedTime * 1000 );
		//printf( "(%.0f ms)\n", test[i] );
	}

	return true;
};

void FinalizeRaytracerTest()
{
	for( unsigned int i = 0; i < RAYTRACER_TEST_RUN_COUNT; i++ )
		delete[] TestRuns[i].HitResult;
}

/*!
 * Raytracer Tests.
 * Assume that cpu raytracer without kdtree generates valid result.
 */
TEST( RaytracerTestCase, CPU_Raytracer_Test )
{
	const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
	//printf("We are in test %s of test case %s.\n", test_info->name(), test_info->test_case_name());

	if( !CPU_LINEAR_1_THREAD_TEST_RUN->Enabled )
		return;

	if( CPU_KDTREE_1_THREAD_TEST_RUN->Enabled )
		ASSERT_EQ( CPU_LINEAR_1_THREAD_TEST_RUN->RayCount, CPU_KDTREE_1_THREAD_TEST_RUN->RayCount ) << "Error: RayCount must be equal.";
	if( CPU_KDTREE_1_THREAD_TEST_RUN->Enabled )
		ASSERT_EQ( CPU_KDTREE_2_THREAD_TEST_RUN->RayCount, CPU_KDTREE_2_THREAD_TEST_RUN->RayCount ) << "Error: RayCount must be equal.";
	if( CPU_KDTREE_4_THREAD_TEST_RUN->Enabled )
		ASSERT_EQ( CPU_LINEAR_1_THREAD_TEST_RUN->RayCount, CPU_KDTREE_4_THREAD_TEST_RUN->RayCount ) << "Error: RayCount must be equal.";
	if( CPU_KDTREE_5_THREAD_TEST_RUN->Enabled )
		ASSERT_EQ( CPU_LINEAR_1_THREAD_TEST_RUN->RayCount, CPU_KDTREE_5_THREAD_TEST_RUN->RayCount ) << "Error: RayCount must be equal.";
	if( CPU_KDTREE_8_THREAD_TEST_RUN->Enabled )
		ASSERT_EQ( CPU_LINEAR_1_THREAD_TEST_RUN->RayCount, CPU_KDTREE_8_THREAD_TEST_RUN->RayCount ) << "Error: RayCount must be equal.";
	if( CPU_KDTREE_16_THREAD_TEST_RUN->Enabled )
		ASSERT_EQ( CPU_LINEAR_1_THREAD_TEST_RUN->RayCount, CPU_KDTREE_16_THREAD_TEST_RUN->RayCount ) << "Error: RayCount must be equal.";

	unsigned int ErrorCount_1Thread = 0;
	unsigned int ErrorCount_2Threads = 0;
	unsigned int ErrorCount_4Threads = 0;
	unsigned int ErrorCount_5Threads = 0;
	unsigned int ErrorCount_8Threads = 0;
	unsigned int ErrorCount_16Threads = 0;

	for( unsigned int i = 0; i < CPU_KDTREE_1_THREAD_TEST_RUN->RayCount; i++ )
	{
		if( !equal( CPU_LINEAR_1_THREAD_TEST_RUN->HitResult[i], CPU_KDTREE_1_THREAD_TEST_RUN->HitResult[i]) )
			ErrorCount_1Thread++;
		if( !equal( CPU_LINEAR_1_THREAD_TEST_RUN->HitResult[i], CPU_KDTREE_2_THREAD_TEST_RUN->HitResult[i]) )
			ErrorCount_2Threads++;
		if( !equal( CPU_LINEAR_1_THREAD_TEST_RUN->HitResult[i], CPU_KDTREE_4_THREAD_TEST_RUN->HitResult[i]) )
			ErrorCount_4Threads++;
		if( !equal( CPU_LINEAR_1_THREAD_TEST_RUN->HitResult[i], CPU_KDTREE_5_THREAD_TEST_RUN->HitResult[i]) )
			ErrorCount_5Threads++;
		if( !equal( CPU_LINEAR_1_THREAD_TEST_RUN->HitResult[i], CPU_KDTREE_8_THREAD_TEST_RUN->HitResult[i]) )
			ErrorCount_8Threads++;
		if( !equal( CPU_LINEAR_1_THREAD_TEST_RUN->HitResult[i], CPU_KDTREE_16_THREAD_TEST_RUN->HitResult[i]) )
			ErrorCount_16Threads++;
	}

	if( CPU_KDTREE_1_THREAD_TEST_RUN->Enabled )
		EXPECT_EQ( ErrorCount_1Thread, 0 ) << CPU_LINEAR_1_THREAD_TEST_RUN->TestRunName << " vs " << CPU_KDTREE_1_THREAD_TEST_RUN->TestRunName ;
	if( CPU_KDTREE_2_THREAD_TEST_RUN->Enabled )
		EXPECT_EQ( ErrorCount_2Threads, 0 ) << CPU_KDTREE_1_THREAD_TEST_RUN->TestRunName << " vs " << CPU_KDTREE_2_THREAD_TEST_RUN->TestRunName;
	if( CPU_KDTREE_4_THREAD_TEST_RUN->Enabled )
		EXPECT_EQ( ErrorCount_4Threads, 0 ) << CPU_KDTREE_1_THREAD_TEST_RUN->TestRunName << " vs " << CPU_KDTREE_4_THREAD_TEST_RUN->TestRunName;
	if( CPU_KDTREE_5_THREAD_TEST_RUN->Enabled )
		EXPECT_EQ( ErrorCount_5Threads, 0 ) << CPU_KDTREE_1_THREAD_TEST_RUN->TestRunName << " vs " << CPU_KDTREE_5_THREAD_TEST_RUN->TestRunName;
	if( CPU_KDTREE_8_THREAD_TEST_RUN->Enabled )
		EXPECT_EQ( ErrorCount_8Threads, 0 ) << CPU_KDTREE_1_THREAD_TEST_RUN->TestRunName << " vs " << CPU_KDTREE_8_THREAD_TEST_RUN->TestRunName;
	if( CPU_KDTREE_16_THREAD_TEST_RUN->Enabled )
		EXPECT_EQ( ErrorCount_16Threads, 0 ) << CPU_KDTREE_1_THREAD_TEST_RUN->TestRunName << " vs " << CPU_KDTREE_16_THREAD_TEST_RUN->TestRunName;
	//printf( "Error: %d/%d\n", RayCount - CPU_KDTREE_vs_GPU_KDTREE_VALID_COUNT, RayCount );
}

TEST( RaytracerTestCase, GPU_Raytracer_Test )
{
	const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
	//printf("We are in test %s of test case %s.\n", test_info->name(), test_info->test_case_name());

	// TODO: CPU_LINEAR_1_THREAD_TEST_RUN 대신 변수로.
	if( !CPU_LINEAR_1_THREAD_TEST_RUN->Enabled )
		return;

	ASSERT_TRUE( CUDA_KDTREE_TEST_RUN->Enabled ) << "Error: CUDA Test Run is disabled.";
	ASSERT_EQ( CPU_LINEAR_1_THREAD_TEST_RUN->RayCount, CUDA_KDTREE_TEST_RUN->RayCount ) << "Error: RayCount must be equal.";

	unsigned int ErrorCount = 0;

	for( unsigned int i = 0; i < CUDA_KDTREE_TEST_RUN->RayCount; i++ )
	{
		if( !equal( CPU_LINEAR_1_THREAD_TEST_RUN->HitResult[i], CUDA_KDTREE_TEST_RUN->HitResult[i]) )
			ErrorCount++;
	}

	EXPECT_EQ( ErrorCount, 0 ) << CPU_LINEAR_1_THREAD_TEST_RUN->TestRunName << " vs " << CUDA_KDTREE_TEST_RUN->TestRunName ;
}

/*!
 * Spherical Harmonics Tests
 */
//TEST( SphericalHarmonicsTestCase, ProjectionTest )
//{
//	EXPECT_TRUE( true );
//}
//
//TEST( SphericalHarmonicsTestCase, 1 )
//{
//	EXPECT_TRUE( true );
//}
