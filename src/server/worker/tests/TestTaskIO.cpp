/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
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
		TaskIODummy(TaksIOType ioType, const ObjectRange & objRange):TaskIO(ioType, objRange) {};
		virtual void runPrepare(void) override {};
		virtual void runAction(void) override {};
		virtual void runPostAction(void) override {};
};

/****************************************************/
TEST(TestTaskIO, isActive_activate)
{
	//build a task
	TaskIODummy task(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));

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
	TaskIODummy taskRead(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));
	TaskIODummy taskWrite(IO_TYPE_WRITE, ObjectRange(ObjectId(10, 20), 5, 10));

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
	TaskIODummy taskRead1(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));
	TaskIODummy taskRead2(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));
	TaskIODummy taskWrite1(IO_TYPE_WRITE, ObjectRange(ObjectId(10, 20), 5, 10));
	TaskIODummy taskWrite2(IO_TYPE_WRITE, ObjectRange(ObjectId(10, 20), 0, 7));

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
	TaskIODummy taskRead1(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));
	TaskIODummy taskRead2(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 5, 5));
	TaskIODummy taskRead3(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 10, 5));

	//check
	EXPECT_TRUE(taskRead1.collide(&taskRead2));
	EXPECT_FALSE(taskRead1.collide(&taskRead3));
}
