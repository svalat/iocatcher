/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include "../TaskScheduler.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
class TaskIODummy : public TaskIO
{
	public:
		TaskIODummy(TaksIOType ioType, const IORange & ioRange):TaskIO(ioType, ioRange) {};
		virtual void runAction(void) override {};
		virtual void runPostAction(void) override {};
};

/****************************************************/
TEST(TestTaskScheduler, constructor)
{
	//build a scheduler
	TaskScheduler sched;
}

/****************************************************/
TEST(TestTaskScheduler, pushTask_single)
{
	//build a scheduler
	TaskScheduler sched;

	//build a task
	TaskIODummy taskRead(IO_TYPE_READ, IORange(0,100));

	//schedule it
	bool canSchedule = sched.pushTask(&taskRead);
	ASSERT_TRUE(canSchedule);
	ASSERT_TRUE(taskRead.isActive());
	ASSERT_FALSE(taskRead.isBlocked());

	//pop it
	TaskVecor toStart;
	sched.popFinishedTask(toStart, &taskRead);
	ASSERT_EQ(0, toStart.size());
}

/****************************************************/
TEST(TestTaskScheduler, pushTask_two_parallel_no_collide)
{
	//build a scheduler
	TaskScheduler sched;

	//build a task
	TaskIODummy taskRead1(IO_TYPE_WRITE, IORange(0,100));
	TaskIODummy taskRead2(IO_TYPE_WRITE, IORange(100,100));

	//schedule first
	bool canSchedule1 = sched.pushTask(&taskRead1);
	ASSERT_TRUE(canSchedule1);
	ASSERT_TRUE(taskRead1.isActive());
	ASSERT_FALSE(taskRead1.isBlocked());

	//schedule second
	bool canSchedule2 = sched.pushTask(&taskRead2);
	ASSERT_TRUE(canSchedule2);
	ASSERT_TRUE(taskRead2.isActive());
	ASSERT_FALSE(taskRead2.isBlocked());

	//pop it
	TaskVecor toStart;
	sched.popFinishedTask(toStart, &taskRead1);
	ASSERT_EQ(0, toStart.size());
	sched.popFinishedTask(toStart, &taskRead2);
	ASSERT_EQ(0, toStart.size());
}

/****************************************************/
TEST(TestTaskScheduler, pushTask_two_parallel)
{
	//build a scheduler
	TaskScheduler sched;

	//build a task
	TaskIODummy taskRead1(IO_TYPE_READ, IORange(0,100));
	TaskIODummy taskRead2(IO_TYPE_READ, IORange(50,100));

	//schedule first
	bool canSchedule1 = sched.pushTask(&taskRead1);
	ASSERT_TRUE(canSchedule1);
	ASSERT_TRUE(taskRead1.isActive());
	ASSERT_FALSE(taskRead1.isBlocked());

	//schedule second
	bool canSchedule2 = sched.pushTask(&taskRead2);
	ASSERT_TRUE(canSchedule2);
	ASSERT_TRUE(taskRead2.isActive());
	ASSERT_FALSE(taskRead2.isBlocked());

	//pop it
	TaskVecor toStart;
	sched.popFinishedTask(toStart, &taskRead1);
	ASSERT_EQ(0, toStart.size());
	sched.popFinishedTask(toStart, &taskRead2);
	ASSERT_EQ(0, toStart.size());
}

/****************************************************/
TEST(TestTaskScheduler, pushTask_two_no_parallel)
{
	//build a scheduler
	TaskScheduler sched;

	//build a task
	TaskIODummy taskWrite1(IO_TYPE_WRITE, IORange(0,100));
	TaskIODummy taskRead2(IO_TYPE_READ, IORange(50,100));

	//schedule first
	bool canSchedule1 = sched.pushTask(&taskWrite1);
	ASSERT_TRUE(canSchedule1);
	ASSERT_TRUE(taskWrite1.isActive());
	ASSERT_FALSE(taskWrite1.isBlocked());

	//schedule second
	bool canSchedule2 = sched.pushTask(&taskRead2);
	ASSERT_FALSE(canSchedule2);
	ASSERT_FALSE(taskRead2.isActive());
	ASSERT_TRUE(taskRead2.isBlocked());

	//pop it
	TaskVecor toStart;
	sched.popFinishedTask(toStart, &taskWrite1);

	//check has scheduled
	ASSERT_EQ(1, toStart.size());
	ASSERT_EQ(&taskRead2, toStart.front());
	ASSERT_TRUE(taskRead2.isActive());
	ASSERT_FALSE(taskRead2.isBlocked());

	//pop it
	toStart.clear();
	sched.popFinishedTask(toStart, &taskRead2);
	ASSERT_EQ(0, toStart.size());
}

/****************************************************/
TEST(TestTaskScheduler, pushTask_three_no_parallel_all_deps)
{
	//build a scheduler
	TaskScheduler sched;

	//build a task
	TaskIODummy taskWrite1(IO_TYPE_WRITE, IORange(0,100));
	TaskIODummy taskWrite2(IO_TYPE_WRITE, IORange(50,100));
	TaskIODummy taskRead3(IO_TYPE_READ, IORange(50,100));

	//schedule first
	bool canSchedule1 = sched.pushTask(&taskWrite1);
	ASSERT_TRUE(canSchedule1);
	ASSERT_TRUE(taskWrite1.isActive());
	ASSERT_FALSE(taskWrite1.isBlocked());

	//schedule second
	bool canSchedule2 = sched.pushTask(&taskWrite2);
	ASSERT_FALSE(canSchedule2);
	ASSERT_FALSE(taskWrite2.isActive());
	ASSERT_TRUE(taskWrite2.isBlocked());

	//schedule second
	bool canSchedule3 = sched.pushTask(&taskRead3);
	ASSERT_FALSE(canSchedule3);
	ASSERT_FALSE(taskRead3.isActive());
	ASSERT_TRUE(taskRead3.isBlocked());

	//pop it
	TaskVecor toStart;
	sched.popFinishedTask(toStart, &taskWrite1);

	//check has scheduled
	ASSERT_EQ(1, toStart.size());
	ASSERT_EQ(&taskWrite2, toStart.front());
	ASSERT_TRUE(taskWrite2.isActive());
	ASSERT_FALSE(taskWrite2.isBlocked());

	//pop it
	toStart.clear();
	sched.popFinishedTask(toStart, &taskWrite2);

	//check has scheduled
	ASSERT_EQ(1, toStart.size());
	ASSERT_EQ(&taskRead3, toStart.front());
	ASSERT_TRUE(taskRead3.isActive());
	ASSERT_FALSE(taskRead3.isBlocked());

	//pop last
	toStart.clear();
	sched.popFinishedTask(toStart, &taskRead3);
}

/****************************************************/
TEST(TestTaskScheduler, pushTask_three_two_parallel)
{
	//build a scheduler
	TaskScheduler sched;

	//build a task
	TaskIODummy taskWrite1(IO_TYPE_WRITE, IORange(0,100));
	TaskIODummy taskRead2(IO_TYPE_READ, IORange(50,100));
	TaskIODummy taskRead3(IO_TYPE_READ, IORange(50,100));

	//schedule first
	bool canSchedule1 = sched.pushTask(&taskWrite1);
	ASSERT_TRUE(canSchedule1);
	ASSERT_TRUE(taskWrite1.isActive());
	ASSERT_FALSE(taskWrite1.isBlocked());

	//schedule second
	bool canSchedule2 = sched.pushTask(&taskRead2);
	ASSERT_FALSE(canSchedule2);
	ASSERT_FALSE(taskRead2.isActive());
	ASSERT_TRUE(taskRead2.isBlocked());

	//schedule second
	bool canSchedule3 = sched.pushTask(&taskRead3);
	ASSERT_FALSE(canSchedule3);
	ASSERT_FALSE(taskRead3.isActive());
	ASSERT_TRUE(taskRead3.isBlocked());

	//pop it
	TaskVecor toStart;
	sched.popFinishedTask(toStart, &taskWrite1);

	//check has scheduled
	ASSERT_EQ(2, toStart.size());
	ASSERT_EQ(&taskRead2, toStart.front());
	ASSERT_TRUE(taskRead2.isActive());
	ASSERT_FALSE(taskRead2.isBlocked());

	//check has scheduled
	ASSERT_EQ(&taskRead3, toStart.back());
	ASSERT_TRUE(taskRead3.isActive());
	ASSERT_FALSE(taskRead3.isBlocked());

	//pop it
	toStart.clear();
	sched.popFinishedTask(toStart, &taskRead2);
	ASSERT_EQ(0, toStart.size());

	//pop it
	toStart.clear();
	sched.popFinishedTask(toStart, &taskRead3);
	ASSERT_EQ(0, toStart.size());
}
