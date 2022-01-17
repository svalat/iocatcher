/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include <thread>
#include "../TaskIO.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
TEST(TestIORange, constructor)
{
	IORange range(0,10);
}

/****************************************************/
TEST(TestIORange, end)
{
	IORange range(10,20);
	EXPECT_EQ(range.end(), 30);
}

/****************************************************/
TEST(TestIORange, collide)
{
	IORange range_5_20(5,15);
	IORange range_10_30(10,20);
	IORange range_30_50(30,20);
	IORange range_35_60(35,25);
	
	//with first range
	EXPECT_TRUE(range_5_20.collide(range_10_30));
	EXPECT_FALSE(range_5_20.collide(range_30_50));
	EXPECT_FALSE(range_5_20.collide(range_35_60));

	//with second range
	EXPECT_FALSE(range_10_30.collide(range_30_50));
	EXPECT_FALSE(range_10_30.collide(range_35_60));

	//with third one
	EXPECT_TRUE(range_30_50.collide(range_35_60));
}
