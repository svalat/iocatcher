/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <cassert>
#include <algorithm>
#include "base/common/Debug.hpp"
#include "TaskScheduler.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Push a task in the scheduler list.
 * If this task can be scheduled (collide with non or can run in parallel with them)
 * @return True if the task can be immediately scheduled, false if it is posponed
 * waiting another task to finish.
**/
bool TaskScheduler::pushTask(TaskIO * task)
{
	//check
	assert(task != NULL);

	//add to the list
	this->tasks.push_back(task);

	//check if can schedule
	return this->canSchedule(task);
}

/****************************************************/
bool TaskScheduler::canSchedulePreparedTask(TaskIO * task)
{
	//check
	assert(task != NULL);
	assert(task->getStage() == STAGE_ACTION);
	assert(std::find(this->tasks.begin(), this->tasks.end(), task) != this->tasks.end());

	//check if can schedule
	return this->canSchedule(task);
}

/****************************************************/
/**
 * @todo: can do an optimization by registering the taks to all colliding one
 * this might consume more memory. To be experimented to see if it is really
 * more efficient.
**/
bool TaskScheduler::canSchedule(TaskIO * task)
{
	//vars
	bool canSchedule = true;

	//check
	assert(task != NULL);

	//The naive algo will
	// - Loop on the list from the end (because there is more chance it is 
	//   the first insertion so be at the end)
	// - Overpass the position of the current task.
	// - Then, from here, find the first colliding and not parallel taks.
	// - If we found one, we register the dependency to this task
	// => when the blocking task finish, we search again in the list to see
	//    if we can schedule the task. As we register to the latest task
	//    in the list, there is chances that previous also blocking deps are finished
	//    so we do not loose time to register to all.
	bool hasSeenCurrent = false;
	for (auto it = this->tasks.rbegin() ; it != this->tasks.rend() ; ++it) {
		//if need to check
		if (hasSeenCurrent) {
			//if they collide and cannot run in parallel we register the dependency
			if (task->collide(*it) && task->canRunInParallel(*it) == false) {
				//cannot schedule & register deps
				canSchedule = false;
				(*it)->registerToUnblock(task);

				//no need to continue
				break;
			}
		} else if (*it == task) {
			//ok we found it now we can check colliding tasks
			hasSeenCurrent = true;
		}
	}

	//if schedule, mark active
	if (canSchedule) {
		assert(task->isBlocked() == false);
		task->activate();
	}

	//return
	return canSchedule;
}

/****************************************************/
void TaskScheduler::removeFromList(TaskIO * task)
{
	//check
	assert(task != NULL);
	assert(task->isActive());

	//find the task in the list
	auto itFind = this->tasks.begin();
	while (*itFind != task) {
		++itFind;
		assumeArg(itFind != this->tasks.end(), "Recive a task which is not in the scheduler task list : %1 !")
			.arg(task)
			.end();
	}

	//remove it from the list
	this->tasks.erase(itFind);
}

/****************************************************/
//TODO otimization by giving multiple task to unregister in one batch
void TaskScheduler::popFinishedTask(TaskVecor & toStart, TaskIO * task)
{
	//check
	assert(task != NULL);
	assert(task->isActive());

	//remove from list
	this->removeFromList(task);

	//look on the inner tasks
	auto & deps = task->getBlockedTasks();
	for (auto & it : deps) {
		//unblock it
		it->unblock();

		//check if we can schedule
		if (this->canSchedule(it)) {
			//check
			assert(it->isBlocked() == false);

			//add to the list
			toStart.push_back(it);
		}
	}
}
