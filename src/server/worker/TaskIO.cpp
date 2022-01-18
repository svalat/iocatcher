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
IORange::IORange(void)
{
	this->address = 0;
	this->size = 0;
}

/****************************************************/
IORanges::IORanges(size_t count)
{
	//check
	assert(count > 0);

	//allocate
	this->ranges = new IORange[count];

	//init
	this->count = count;
	this->cursor = 0;
}

/****************************************************/
IORanges::IORanges(const IORange & uniqRange)
{
	//setup
	this->ranges = new IORange[1];
	this->ranges[0] = uniqRange;
	this->count = 1;
	this->cursor = 1;
}

/****************************************************/
IORanges::IORanges(IORanges && orig)
{
	//move
	this->ranges = orig.ranges;
	this->cursor = orig.cursor;
	this->count = orig.count;

	//reset orig
	orig.ranges = NULL;
	orig.count = 0;
	orig.cursor = 0;
}

/****************************************************/
IORanges::IORanges(const IORanges & orig)
{
	//setup
	this->ranges = new IORange[orig.count];
	this->count = orig.count;
	this->cursor = orig.cursor;

	//copy
	for (int i = 0 ; i < this->cursor ; i++)
		this->ranges[i] = orig.ranges[i];
}

/****************************************************/
IORanges::~IORanges(void)
{
	if (this->ranges != NULL)
		delete [] this->ranges;
}

/****************************************************/
bool IORanges::ready(void) const
{
	return this->ranges != NULL && this->cursor == this->count && this->count > 0;
}

/****************************************************/
IORanges & IORanges::push(size_t address, size_t size)
{
	//check
	assert(address != 0);
	assert(size != 0);
	assert(this->cursor < this->count);

	//push
	this->ranges[this->cursor].address = address;
	this->ranges[this->cursor].size = size;

	//move
	this->cursor++;

	//ret for chaining
	return *this;
}

/****************************************************/
IORanges & IORanges::push(const IORange & range)
{
	//check
	assert(range.address != 0);
	assert(range.size != 0);
	assert(this->cursor < this->count);

	//push
	this->ranges[this->cursor] = range;

	//move
	this->cursor++;

	//ret for chaining
	return *this;
}

/****************************************************/
bool IORanges::collide(const IORanges & ranges) const
{
	//check
	assert(this->cursor == this->count);
	assert(ranges.cursor == ranges.count);

	//search for collide
	//@TODO make an optimisation by sorting them
	for (int i = 0 ; i < this->count ; i++)
		for (int j = 0 ; j < ranges.count; j++)
			if (this->ranges[i].collide(ranges.ranges[j]))
				return true;

	//no collision
	return false;
}

/****************************************************/
TaskIO::TaskIO(TaksIOType ioType, const IORanges & ioRanges)
       :ioRanges(ioRanges)
{
	//check
	assert(ioRanges.ready());
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
