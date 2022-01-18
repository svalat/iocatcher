/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include "../TaskIO.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
class TaskIODummy : public TaskIO
{
	public:
		TaskIODummy(TaksIOType ioType, const IORanges & ioRanges):TaskIO(ioType, ioRanges) {};
		virtual void runAction(void) override {};
		virtual void runPostAction(void) override {};
};

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

/****************************************************/
TEST(TestTaskIO, isActive_activate)
{
	//build a task
	TaskIODummy task(IO_TYPE_READ, IORange(0, 10));

	//check
	ASSERT_FALSE(task.isActive());

	//activate
	task.activate();

	//check has changed
	ASSERT_TRUE(task.isActive());
}

/****************************************************/
TEST(TestTaskIO, registerToUnblock)
{
	//build two task which collide and cannot run in parallel
	TaskIODummy taskRead(IO_TYPE_READ, IORange(0, 10));
	TaskIODummy taskWrite(IO_TYPE_WRITE, IORange(5, 10));

	//check
	ASSERT_EQ(0, taskRead.getBlockedTasks().size());

	//register blocked
	taskRead.registerToUnblock(&taskWrite);

	//check again
	ASSERT_EQ(1, taskRead.getBlockedTasks().size());
	ASSERT_EQ(&taskWrite, taskRead.getBlockedTasks().front());
}

/****************************************************/
TEST(TestTaskIO, canRunInParallel)
{
	//build two task which collide and cannot run in parallel
	TaskIODummy taskRead1(IO_TYPE_READ, IORange(0, 10));
	TaskIODummy taskRead2(IO_TYPE_READ, IORange(0, 10));
	TaskIODummy taskWrite1(IO_TYPE_WRITE, IORange(5, 10));
	TaskIODummy taskWrite2(IO_TYPE_WRITE, IORange(0, 7));

	//check
	EXPECT_TRUE(taskRead1.canRunInParallel(&taskRead2));
	EXPECT_FALSE(taskRead1.canRunInParallel(&taskWrite1));
	EXPECT_FALSE(taskRead1.canRunInParallel(&taskWrite2));
	EXPECT_FALSE(taskWrite1.canRunInParallel(&taskWrite2));
}

/****************************************************/
TEST(TestTaskIO, collide)
{
	//build two task which collide and cannot run in parallel
	TaskIODummy taskRead1(IO_TYPE_READ, IORange(0, 10));
	TaskIODummy taskRead2(IO_TYPE_READ, IORange(5, 5));
	TaskIODummy taskRead3(IO_TYPE_READ, IORange(10, 5));

	//check
	EXPECT_TRUE(taskRead1.collide(&taskRead2));
	EXPECT_FALSE(taskRead1.collide(&taskRead3));
}
