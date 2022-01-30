/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <cassert>
#include "base/common/Debug.hpp"
#include "TaskRunner.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the task runner, it just init the two sub objects 
 * (worker manager and task scheduler).
**/
TaskRunner::TaskRunner(int workers, LibfabricConnection * connection)
           :workerManager(workers, connection)
{
	this->pendingTasks = 0;
}

/****************************************************/
void TaskRunner::pushTask(TaskIO * task)
{
	//check
	assert(task != NULL);

	//increment
	this->pendingTasks++;

	//check if can run the task or register it into the scheduler
	//if OK, send it to the worker manager to run it
	if (this->taskScheduler.pushTask(task))
		this->runPrepareAndSchedule(task);
}

/****************************************************/
void TaskRunner::runPrepareAndSchedule(TaskIO * task)
{
	//check
	assert(task != NULL);

	//ready state
	bool ready = true;

	//check if prepare stage need to be done
	if (task->getStage() == STAGE_PREPARE) {
		//run prepare
		task->runNextStage(STAGE_PREPARE);

		//check again schedulability
		//Remark, this is required is we collide on addresse ranges
		//which should append only if we play with COW
		ready = this->taskScheduler.canSchedulePreparedTask(task);
	}

	//send it to workers
	if (ready)
		this->workerManager.pushTask(task);
}

/****************************************************/
/** @todo make bulk request instead of one by one. **/
//poll a task
int TaskRunner::schedule(void)
{
	//vars
	int count = 0;

	//make a first poll
	Task * task = this->workerManager.pollFinishedTask(false);

	//while we have tasks to poll
	while (task != NULL) {
		//increment executed tasks
		count++;

		//cast
		TaskIO * ioTask = dynamic_cast<TaskIO*>(task);

		//call end operation
		bool finished = ioTask->runNextStage(STAGE_POST);
		assert(finished);

		//look for schedule
		TaskVecor toStart;
		taskScheduler.popFinishedTask(toStart, ioTask);

		//schedule all
		for (auto & it : toStart) {
			this->runPrepareAndSchedule(it);
		}

		//delete the task & decrement
		delete task;
		this->pendingTasks--;
		assert(this->pendingTasks >= 0);

		//poll again
		task = this->workerManager.pollFinishedTask(false);
	}

	//return
	return count;
}

/****************************************************/
void TaskRunner::waitAllFinished(void)
{
	//schedule until there not anymore pending tasks to run
	while (this->pendingTasks > 0)
		this->schedule();
}