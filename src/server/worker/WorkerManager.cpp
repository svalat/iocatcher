/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <cassert>
#include "base/common/Debug.hpp"
#include "WorkerManager.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the task manager. It start the worker thread.
 * @param worker The number of worker to spawn.
 * @param connection The connection to signal if we use passive polling.
**/
WorkerManager::WorkerManager(int workers, LibfabricConnection * connection)
              :tasksIn(true)
              ,tasksOut(false)
{
	//debug
	IOC_DEBUG_ARG("worker", "Manager configured to launch %1 workers").arg(workers).end();

	//set
	this->connection = connection;

	//start the N threads
	for (int i = 0 ; i < workers ; i++)
		this->workers.emplace_back(&this->tasksIn, &this->tasksOut, connection);
}

/****************************************************/
/**
 * Destructor of the worker manager, it sends EXIT task for each worker.
**/
WorkerManager::~WorkerManager(void)
{
	//stop all the threads
	for (size_t i = 0 ; i < this->workers.size() ; i++)
		this->tasksIn.push(WORKER_EXIT_TASK);
}

/****************************************************/
/**
 * Push a task to be ran by a worker.
**/
void WorkerManager::pushTask(Task * task)
{
	//check
	assert(task != NULL);
	assert(task != WORKER_EXIT_TASK);

	//check if has worker
	if (this->workers.empty() || task->isImmediate()) {
		//if not worker we execute immediately
		task->runNextStage(STAGE_ACTION);
		this->tasksOut.push(task);
		//wake up the connection polling function if passive
		if (this->connection != NULL)
			this->connection->signalPassivePolling();
	} else {
		//if we have workers, we push in the work queue.
		this->tasksIn.push(task);
	}
}

/****************************************************/
/**
 * Poll for a finish task to run the post action in the network handling thread.
 * @param waitOne Make actively waiting to a new task.
 * @return Return a pointer to a finished task of NULL.
**/
Task * WorkerManager::pollFinishedTask(bool waitOne)
{
	//var
	Task * task = NULL;

	//check mode
	if (waitOne) {
		//active poll to get a task
		while (task == NULL)
			task = this->tasksOut.pop();
	} else {
		task = this->tasksOut.pop();
	}

	//ok
	return task;
}
