/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include <thread>
#include "../Worker.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
class TestWorkerTask : public Task {
	public:
		TestWorkerTask(void) {this->ran = false;};
		virtual void runAction(void) override {this->ran = true;};
		virtual void runPostAction(void) override {};
		bool ran;
};

/****************************************************/
TEST(TestWorker, constructor)
{
	WorkerTaskQueue in(true);
	WorkerTaskQueue out(false);
	Worker worker(&in, &out);
	in.push(WORKER_EXIT_TASK);
}

/****************************************************/
TEST(TestWorker, run_task)
{
	//build
	WorkerTaskQueue in(true);
	WorkerTaskQueue out(false);
	Worker worker(&in, &out);

	//push task
	TestWorkerTask task;
	in.push(&task);

	//active wait
	Task * done = NULL;
	do { done = out.pop(); } while (done == NULL);

	//check
	EXPECT_EQ(&task, done);
	EXPECT_TRUE(task.ran);

	//stop the worker
	in.push(WORKER_EXIT_TASK);
}
