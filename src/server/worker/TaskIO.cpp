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
TaskIO::TaskIO(TaksIOType ioType, IORange ioRange)
       :ioRange(ioRange)
{
	//check
	assert(ioRange.size > 0);
	assert(ioType == IO_TYPE_READ || ioType == IO_TYPE_WRITE);

	//init
	this->ioType = ioType;
	this->active = false;
	this->toUnlock = NULL;
	this->blockingDependencies = 0;

	//the schedule group is a self circular list by default
	this->schedGroupNext = this;
}

/****************************************************/
void TaskIO::registerInScheduleHierarchy(TaskIO * task)
{
	//check
	assert(task != NULL);
	assert(this->ioRange.collide(task->ioRange));
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
