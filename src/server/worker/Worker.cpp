/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <cassert>
#include "base/common/Debug.hpp"
#include "Worker.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the worker.
 * @param inQueue Define the queue from which to poll the incoming tasks.
 * @param outQueue Push the task in the given queue when they are finished.
**/
Worker::Worker(WorkerTaskQueue * inQueue, WorkerTaskQueue * outQueue)
{
	//check
	assert(inQueue != NULL);
	assert(outQueue != NULL);

	//spaw the thread
	this->thread = std::thread([this, inQueue, outQueue](){
		//debug
		IOC_DEBUG_ARG("worker", "Worker thread %1 started").arg(this).end();

		//loop until get an exit task
		while (true) {
			//pop a task
			Task * task = inQueue->pop();

			//If get an exit task, exit
			if (task == WORKER_EXIT_TASK)
				return;

			//run it
			task->runAction();

			//push it to the out queue
			outQueue->push(task);
		}
	});
}

/****************************************************/
Worker::~Worker(void)
{
	//wait the worker thread
	this->thread.join();
}
