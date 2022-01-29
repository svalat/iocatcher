/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include "../ObjectRanges.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
TEST(TestObjectRange, constructor)
{
	ObjectRange range(ObjectId(10,20), 0, 10);
}

/****************************************************/
TEST(TestObjectRange, collide)
{
	ObjectRange range_5_20(ObjectId(10,20), 5,15);
	ObjectRange range_5_20_other(ObjectId(10,21), 5,15);
	ObjectRange range_10_30(ObjectId(10,20), 0,20);
	ObjectRange range_30_50(ObjectId(10,20), 30,20);
	ObjectRange range_35_60(ObjectId(10,20), 35,25);
	
	//with first range
	EXPECT_TRUE(range_5_20.collide(range_10_30));
	EXPECT_FALSE(range_5_20.collide(range_5_20_other));
	EXPECT_FALSE(range_5_20.collide(range_30_50));
	EXPECT_FALSE(range_5_20.collide(range_35_60));

	//with second range
	EXPECT_FALSE(range_10_30.collide(range_30_50));
	EXPECT_FALSE(range_10_30.collide(range_35_60));

	//with third one
	EXPECT_TRUE(range_30_50.collide(range_35_60));
}

/****************************************************/
TEST(TestObjectRanges, constructor)
{
	ObjectRanges ranges(1);
}

/****************************************************/
TEST(TestObjectRanges, push)
{
	ObjectRanges ranges(4);
	ranges.push(ObjectRange(ObjectId(10, 20), 1, 10));
	ranges.push(ObjectId(10, 20), 1, 10);
	ranges.push(ObjectId(10, 20), 20, 10).push(ObjectId(10, 20), 30,5);
}

/****************************************************/
TEST(TestObjectRanges, collide)
{
	//first range
	ObjectRanges ranges1(2);
	ranges1.push(ObjectId(10, 20), 100,5);
	ranges1.push(ObjectId(10, 20), 105,5);

	//second
	ObjectRanges ranges2(1);
	ranges2.push(ObjectId(10, 20), 103, 5);

	//second
	ObjectRanges ranges3(3);
	ranges3.push(ObjectId(10, 20), 20, 5);
	ranges3.push(ObjectId(10, 20), 25, 5);
	ranges3.push(ObjectId(10, 20), 30, 5);

	//second
	ObjectRanges ranges4(1);
	ranges4.push(ObjectId(10, 21), 100,5);

	//check collide
	EXPECT_TRUE(ranges1.collide(ranges2));
	EXPECT_FALSE(ranges1.collide(ranges3));
	EXPECT_FALSE(ranges1.collide(ranges4));
	EXPECT_FALSE(ranges2.collide(ranges3));
}
