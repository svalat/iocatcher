/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_TASK_RUNNER_HPP
#define IOC_TASK_RUNNER_HPP

/****************************************************/
#include <list>
#include <vector>
#include "TaskScheduler.hpp"
#include "WorkerManager.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Associate a task scheduler and a worker manager to effectively run the tasks.
**/
class TaskRunner
{
	public:
		TaskRunner(int workers, LibfabricConnection * connection = NULL);
		void pushTask(TaskIO * task);
		int schedule(void);
		void waitAllFinished(void);
		void terminateDetachedPost(TaskIO * task);
	private:
		void runPrepareAndSchedule(TaskIO * task);
	private:
		TaskScheduler taskScheduler;
		WorkerManager workerManager;
		int pendingTasks;
};

}

#endif //IOC_TASK_SCHEDULER_HPP
