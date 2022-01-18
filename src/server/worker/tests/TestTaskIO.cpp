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
