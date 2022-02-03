/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
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
