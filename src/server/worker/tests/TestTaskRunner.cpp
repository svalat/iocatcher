/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
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
class TaskIODummy : public TaskIO
{
	public:
		TaskIODummy(TaksIOType ioType, const IORange & ioRange):TaskIO(ioType, ioRange) {this->ranAc = this->ranPost = false;};
		~TaskIODummy(void) {EXPECT_TRUE(this->ranAc); EXPECT_TRUE(this->ranPost); gblDone++;};
		virtual void runAction(void) override {this->ranAc = true;};
		virtual void runPostAction(void) override {this->ranPost = true;};
	private:
		bool ranAc;
		bool ranPost;
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
		runner.pushTask(new TaskIODummy((i%4 == 0)?IO_TYPE_WRITE:IO_TYPE_READ, IORange(0,100)));

	//wait all to be done
	runner.waitAllFinished();

	//check
	EXPECT_EQ(cnt, gblDone);
}
