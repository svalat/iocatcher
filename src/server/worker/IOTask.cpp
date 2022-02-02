/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <cassert>
#include "base/common/Debug.hpp"
#include "IOTask.hpp"
#include "TaskRunner.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
IOTask::IOTask(IOTaksType ioType, int objectRangesCount)
       :memRanges(0)
       ,objectRanges(objectRangesCount)
{
	//check
	assert(ioType == IO_TYPE_READ || ioType == IO_TYPE_WRITE);

	//init
	this->ioType = ioType;
	this->active = false;
	this->blockingDependencies = 0;
	this->taskRunner = NULL;
	this->detachPost = false;
}

/****************************************************/
IOTask::IOTask(IOTaksType ioType, const ObjectRange & objectRange)
       :memRanges(0)
       ,objectRanges(objectRange)
{
	//check
	assert(ioType == IO_TYPE_READ || ioType == IO_TYPE_WRITE);

	//init
	this->ioType = ioType;
	this->active = false;
	this->blockingDependencies = 0;
	this->taskRunner = NULL;
	this->detachPost = false;
}

/****************************************************/
IOTask::IOTask(IOTaksType ioType, const ObjectRange & objectRange, const IORanges & memRanges)
       :memRanges(memRanges)
       ,objectRanges(objectRange)
{
	//check
	assert(ioType == IO_TYPE_READ || ioType == IO_TYPE_WRITE);

	//init
	this->ioType = ioType;
	this->active = false;
	this->blockingDependencies = 0;
	this->taskRunner = NULL;
	this->detachPost = false;
}

/****************************************************/
inline bool IOTask::oneIs(const IOTask * task1, const IOTask * task2, IOTaksType type)
{
	return (task1->ioType == type || task2->ioType == type);
}

/****************************************************/
inline bool IOTask::oneOrTheOtherIs(const IOTask * task1, const IOTask * task2, IOTaksType type1, IOTaksType type2)
{
	return (task1->ioType == type1 && task2->ioType == type2)
	    || (task1->ioType == type2 && task2->ioType == type1);
}

/****************************************************/
inline bool IOTask::both(const IOTask * task1, const IOTask * task2, IOTaksType type)
{
	return (task1->ioType == type && task2->ioType == type);
}

/****************************************************/
bool IOTask::canRunInParallel(const IOTask * task) const
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
void IOTask::registerToUnblock(IOTask * task)
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
std::deque<IOTask*> & IOTask::getBlockedTasks(void)
{
	return this->toUnblock;
}

/****************************************************/
bool IOTask::isActive(void) const
{
	return this->active;
};

/****************************************************/
void IOTask::activate(void)
{
	this->active = true;
};

/****************************************************/
bool IOTask::unblock(void)
{
	//check
	assume(this->blockingDependencies > 0, "Try to unblock a task which is already unblocked !");

	//decrement
	this->blockingDependencies--;

	//return true if unblocked
	return (this->blockingDependencies == 0);
}

/****************************************************/
void IOTask::setMemRanges(IORanges && memRanges)
{
	this->memRanges = std::move(memRanges);
}

/****************************************************/
void IOTask::pushObjectRange(const ObjectRange & objectRange)
{
	this->objectRanges.push(objectRange);
}

/****************************************************/
void IOTask::setDetachedPost(void)
{
	this->detachPost = true;
}

/****************************************************/
bool IOTask::isDetachedPost(void) const
{
	return this->detachPost;
}

/****************************************************/
void IOTask::setTaskRunner(TaskRunner * runner)
{
	this->taskRunner = runner;
}

/****************************************************/
void IOTask::terminateDetachedPost(void)
{
	assert(this->detachPost);
	assert(this->taskRunner != NULL);
	this->taskRunner->terminateDetachedPost(this);
}
