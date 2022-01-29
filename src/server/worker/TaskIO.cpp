/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

/****************************************************/
#include <cassert>
#include "base/common/Debug.hpp"
#include "TaskIO.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
TaskIO::TaskIO(TaksIOType ioType, const ObjectRange & objectRange)
       :memRanges(0)
       ,objectRanges(objectRange)
{
	//check
	assert(ioType == IO_TYPE_READ || ioType == IO_TYPE_WRITE);

	//init
	this->ioType = ioType;
	this->active = false;
	this->blockingDependencies = 0;
}

/****************************************************/
TaskIO::TaskIO(TaksIOType ioType, const ObjectRange & objectRange, const IORanges & memRanges)
       :memRanges(memRanges)
       ,objectRanges(objectRange)
{
	//check
	assert(ioType == IO_TYPE_READ || ioType == IO_TYPE_WRITE);

	//init
	this->ioType = ioType;
	this->active = false;
	this->blockingDependencies = 0;
}

/****************************************************/
inline bool TaskIO::oneIs(const TaskIO * task1, const TaskIO * task2, TaksIOType type)
{
	return (task1->ioType == type || task2->ioType == type);
}

/****************************************************/
inline bool TaskIO::oneOrTheOtherIs(const TaskIO * task1, const TaskIO * task2, TaksIOType type1, TaksIOType type2)
{
	return (task1->ioType == type1 && task2->ioType == type2)
	    || (task1->ioType == type2 && task2->ioType == type1);
}

/****************************************************/
inline bool TaskIO::both(const TaskIO * task1, const TaskIO * task2, TaksIOType type)
{
	return (task1->ioType == type && task2->ioType == type);
}

/****************************************************/
bool TaskIO::canRunInParallel(const TaskIO * task) const
{
	//check
	assert(task != NULL);
	assert(task->ioType == IO_TYPE_READ || task->ioType == IO_TYPE_WRITE);
	assert(this->ioType == IO_TYPE_READ || this->ioType == IO_TYPE_WRITE);

	//check what is allowed or not
	if (this->oneIs(this, task, IO_TYPE_WRITE)) {//if write nothing in parallel
		return false;
	} else if (this->both(this, task, IO_TYPE_READ)) { //both read, ok in parallel
		return true;
	} else {//this should not happen
		IOC_FATAL_ARG("This situation should never happen : %1 collide with %2").arg(this->ioType).arg(task->ioType).end();
		return false;
	}
}

/****************************************************/
void TaskIO::registerToUnblock(TaskIO * task)
{
	//check
	assert(this != NULL);
	assert(task != NULL);
	assert(this->collide(task));
	assert(this->canRunInParallel(task) == false);

	//register in list
	this->toUnblock.push_back(task);

	//increment the blocking counter on the remote task
	task->blockingDependencies++;
}

/****************************************************/
std::deque<TaskIO*> & TaskIO::getBlockedTasks(void)
{
	return this->toUnblock;
}

/****************************************************/
bool TaskIO::isActive(void) const
{
	return this->active;
};

/****************************************************/
void TaskIO::activate(void)
{
	this->active = true;
};

/****************************************************/
bool TaskIO::unblock(void)
{
	//check
	assume(this->blockingDependencies > 0, "Try to unblock a task which is already unblocked !");

	//decrement
	this->blockingDependencies--;

	//return true if unblocked
	return (this->blockingDependencies == 0);
}
