/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_WORKER_MANAGER_HPP
#define IOC_WORKER_MANAGER_HPP

/****************************************************/
#include <list>
#include "Worker.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
class WorkerManager
{
	public:
		WorkerManager(int workers, LibfabricConnection * connection = NULL);
		~WorkerManager(void);
		void pushTask(Task * task);
		Task * pollFinishedTask(bool waitOne);
	private:
		/** Keep track of the incomming tasks to dispatch to the workers. **/
		WorkerTaskQueue tasksIn;
		/** 
		 * Task which has run in the worker thread and in wait for the post action in the
		 * networking thread.
		**/
		WorkerTaskQueue tasksOut;
		/** List of active threads. **/
		std::list<Worker> workers;
		/** keep track of the connection to signal if we are running without workers. **/
		LibfabricConnection * connection;
};

}

#endif //IOC_WORKER_MANAGER_HPP
