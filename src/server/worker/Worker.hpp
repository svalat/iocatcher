/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

#ifndef IOC_WORKER_HPP
#define IOC_WORKER_HPP

/****************************************************/
#include <thread>
#include "Task.hpp"
#include "SharedQueue.hpp"
#include "base/network/LibfabricConnection.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
#define WORKER_EXIT_TASK ((Task*)(NULL))

/****************************************************/
typedef SharedQueue<Task> WorkerTaskQueue;

/****************************************************/
class Worker
{
	public:
		Worker(WorkerTaskQueue * inQueue, WorkerTaskQueue * outQueue, LibfabricConnection * connection = NULL);
		~Worker(void);
	private:
		/** Handler to thread thread running the worker. **/
		std::thread thread;
};

}

#endif //IOC_WORKER_HPP
