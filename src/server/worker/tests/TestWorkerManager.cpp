/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include <thread>
#include "../WorkerManager.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
class TestWorkerManagerTask : public Task {
	public:
		TestWorkerManagerTask(void) {this->ran = false;this->runNextStage(STAGE_PREPARE);};
		~TestWorkerManagerTask(void) {this->runNextStage(STAGE_POST);};
		virtual void runPrepare(void) override {};
		virtual void runAction(void) override {this->ran = true;};
		virtual void runPostAction(void) override {};
		bool ran;
};

/****************************************************/
TEST(TestWorkerManager, constructor_one)
{
	WorkerManager wmanager(1);
}

/****************************************************/
TEST(TestWorkerManager, constructor_many)
{
	WorkerManager wmanager(8);
}

/****************************************************/
TEST(TestWorkerManager, poll_null)
{
	WorkerManager wmanager(1);
	EXPECT_EQ(nullptr, wmanager.pollFinishedTask(false));
}

/****************************************************/
TEST(TestWorkerManager, run_on_task)
{
	//vars
	WorkerManager wmanager(1);
	TestWorkerManagerTask task;

	//push a task
	wmanager.pushTask(&task);

	//wait to be done
	Task * done = wmanager.pollFinishedTask(true);

	//check
	EXPECT_EQ(&task, done);
	EXPECT_TRUE(task.ran);
}

/****************************************************/
TEST(TestWorkerManager, run_many)
{
	//vars
	WorkerManager wmanager(8);
	const int cnt = 1000;

	//push a task
	for (int i = 0 ; i < cnt ; i++ )
		wmanager.pushTask(new TestWorkerManagerTask);

	//wait to be done
	for (int i = 0 ; i < cnt ; i++ ) {
		TestWorkerManagerTask * done = dynamic_cast<TestWorkerManagerTask*>(wmanager.pollFinishedTask(true));
		EXPECT_TRUE(done->ran);
	}
}

/****************************************************/
TEST(TestWorkerManager, connection_signal)
{
	//start a listening server
	LibfabricDomain domain("localhost", "8668", true);
	LibfabricConnection connection(&domain, true);

	//start worker
	WorkerManager wmanager(1, &connection);
	TestWorkerManagerTask task;

	//make it wait
	std::thread connectionThread([&connection, &wmanager](){
		//make passive polling, this should sleep until the task has finished
		connection.poll(false);
		//pop the task and check its state
		TestWorkerManagerTask * task = dynamic_cast<TestWorkerManagerTask*>(wmanager.pollFinishedTask(false));
		//check
		EXPECT_NE(nullptr, task);
		if (task != NULL) {
			EXPECT_TRUE(task->ran);
		}
	});

	//push the task
	wmanager.pushTask(&task);

	//wait connection thread, it should wake up
	connectionThread.join();

	//check
	EXPECT_TRUE(task.ran);
}
