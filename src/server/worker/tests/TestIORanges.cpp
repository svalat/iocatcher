/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
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

/*********************************************************/
TEST(TestIORange, operator_equal)
{
	IORange range1(0, 100);
	IORange range2(0, 100);
	IORange range3(50, 100);
	IORange range4(0, 50);

	EXPECT_TRUE(range1 == range2);
	EXPECT_FALSE(range1 == range3);
	EXPECT_FALSE(range1 == range4);
}

/*********************************************************/
TEST(TestIORange, intersect_left)
{
	IORange res = IORange::intersect(IORange(0,15), IORange(10, 10));
	EXPECT_EQ(IORange(10, 5), res);
}

/*********************************************************/
TEST(TestIORange, intersect_middle)
{
	IORange res = IORange::intersect(IORange(12,5), IORange(10, 10));
	EXPECT_EQ(IORange(12, 5), res);
}

/*********************************************************/
TEST(TestIORange, intersect_right)
{
	IORange res = IORange::intersect(IORange(15,20), IORange(10, 10));
	EXPECT_EQ(IORange(15, 5), res);
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
