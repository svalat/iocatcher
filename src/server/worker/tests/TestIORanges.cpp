/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include "../IORanges.hpp"

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

/****************************************************/
TEST(TestIORanges, constructor)
{
	IORanges ranges(1);
}

/****************************************************/
TEST(TestIORanges, push)
{
	IORanges ranges(4);
	ranges.push(IORange(1, 10));
	ranges.push(1, 10);
	ranges.push(20, 10).push(30,5);
}

/****************************************************/
TEST(TestIORanges, collide)
{
	//first range
	IORanges ranges1(2);
	ranges1.push(100,5);
	ranges1.push(105,5);

	//second
	IORanges ranges2(1);
	ranges2.push(103, 5);

	//second
	IORanges ranges3(3);
	ranges3.push(20, 5);
	ranges3.push(25, 5);
	ranges3.push(30, 5);

	//check collide
	EXPECT_TRUE(ranges1.collide(ranges2));
	EXPECT_FALSE(ranges1.collide(ranges3));
	EXPECT_FALSE(ranges2.collide(ranges3));
}
