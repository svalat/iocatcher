/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include "../IOTask.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
class IOTaskDummy : public IOTask
{
	public:
		IOTaskDummy(IOTaksType ioType, const ObjectRange & objRange):IOTask(ioType, objRange) {};
		virtual void runPrepare(void) override {};
		virtual void runAction(void) override {};
		virtual void runPostAction(void) override {};
};

/****************************************************/
TEST(TestIOTask, isActive_activate)
{
	//build a task
	IOTaskDummy task(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));

	//check
	ASSERT_FALSE(task.isActive());

	//activate
	task.activate();

	//check has changed
	ASSERT_TRUE(task.isActive());
}

/****************************************************/
TEST(TestIOTask, registerToUnblock)
{
	//build two task which collide and cannot run in parallel
	IOTaskDummy taskRead(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));
	IOTaskDummy taskWrite(IO_TYPE_WRITE, ObjectRange(ObjectId(10, 20), 5, 10));

	//check
	ASSERT_EQ(0, taskRead.getBlockedTasks().size());

	//register blocked
	taskRead.registerToUnblock(&taskWrite);

	//check again
	ASSERT_EQ(1, taskRead.getBlockedTasks().size());
	ASSERT_EQ(&taskWrite, taskRead.getBlockedTasks().front());
}

/****************************************************/
TEST(TestIOTask, canRunInParallel)
{
	//build two task which collide and cannot run in parallel
	IOTaskDummy taskRead1(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));
	IOTaskDummy taskRead2(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));
	IOTaskDummy taskWrite1(IO_TYPE_WRITE, ObjectRange(ObjectId(10, 20), 5, 10));
	IOTaskDummy taskWrite2(IO_TYPE_WRITE, ObjectRange(ObjectId(10, 20), 0, 7));

	//check
	EXPECT_TRUE(taskRead1.canRunInParallel(&taskRead2));
	EXPECT_FALSE(taskRead1.canRunInParallel(&taskWrite1));
	EXPECT_FALSE(taskRead1.canRunInParallel(&taskWrite2));
	EXPECT_FALSE(taskWrite1.canRunInParallel(&taskWrite2));
}

/****************************************************/
TEST(TestIOTask, collide)
{
	//build two task which collide and cannot run in parallel
	IOTaskDummy taskRead1(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0, 10));
	IOTaskDummy taskRead2(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 5, 5));
	IOTaskDummy taskRead3(IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 10, 5));

	//check
	EXPECT_TRUE(taskRead1.collide(&taskRead2));
	EXPECT_FALSE(taskRead1.collide(&taskRead3));
}
