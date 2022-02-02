/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include "../TaskRunner.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
static int gblDone = 0;

/****************************************************/
class TaskIODummy : public IOTask
{
	public:
		TaskIODummy(IOTaksType ioType, const ObjectRange & objRange):IOTask(ioType, objRange) {this->ranAc = this->ranPost = false;};
		~TaskIODummy(void) {EXPECT_TRUE(this->ranAc); EXPECT_TRUE(this->ranPost); gblDone++;};
	protected:
		virtual void runPrepare(void) override {};
		virtual void runAction(void) override {this->ranAc = true;};
		virtual void runPostAction(void) override {this->ranPost = true;};
	public:
		bool ranAc;
		bool ranPost;
};

/****************************************************/
class TaskIODummyDetached : public TaskIODummy
{
	public:
		TaskIODummyDetached(IOTaksType ioType, const ObjectRange & objRange):TaskIODummy(ioType, objRange) {};
	protected:
		virtual void runPostAction(void) override {
			this->setDetachedPost();
			std::thread thread([this](){
				sleep(1);
				this->ranPost = true;
				this->terminateDetachedPost();
			});
			thread.detach();
		};
};

/****************************************************/
TEST(TestTaskRunner, constructor)
{
	TaskRunner runner(8, NULL);
}

/****************************************************/
TEST(TestTaskRunner, run_many)
{
	//vars
	TaskRunner runner(4);
	const int cnt = 1000;

	//reset global var
	gblDone = 0;

	//push a task
	//we build 3 READ then 1 WRITE for force a "complex" scheduling
	for (int i = 0 ; i < cnt ; i++ )
		runner.pushTask(new TaskIODummy((i%4 == 0)?IO_TYPE_WRITE:IO_TYPE_READ, ObjectRange(ObjectId(10, 20), 0,100)));

	//wait all to be done
	runner.waitAllFinished();

	//check
	EXPECT_EQ(cnt, gblDone);
}

/****************************************************/
TEST(TestTaskRunner, run_detached_post)
{
	//vars
	TaskRunner runner(1);

	//reset global var
	gblDone = 0;

	//push a task
	//we build 3 READ then 1 WRITE for force a "complex" scheduling
	runner.pushTask(new TaskIODummyDetached(IO_TYPE_WRITE, ObjectRange(ObjectId(10, 20), 100, 200)));

	//wait all to be done
	runner.waitAllFinished();

	//check
	EXPECT_EQ(1, gblDone);
}
