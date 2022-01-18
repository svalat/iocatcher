/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

#ifndef IOC_TASK_IO_HPP
#define IOC_TASK_IO_HPP

/****************************************************/
#include <cstdlib>
#include <deque>
#include "Task.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
enum TaksIOType
{
	/** Read IO has the property to be performed in parallel on the same segment. **/
	IO_TYPE_READ,
	/** Write IO are exclusive on a given segment and exclusite with READ. **/
	IO_TYPE_WRITE
};

/****************************************************/
struct IORange
{
	IORange(void);
	inline IORange(size_t address, size_t size);
	inline bool collide(const IORange & range) const;
	inline size_t end(void) const;
	size_t address;
	size_t size;
};

/****************************************************/
class IORanges
{
	public:
		IORanges(IORanges && orig);
		IORanges(const IORanges & orig);
		IORanges(size_t count);
		IORanges(const IORange & uniqRange);
		~IORanges(void);
		IORanges & push(const IORange & range);
		IORanges & push(size_t address, size_t size);
		bool collide(const IORanges & ranges) const;
		bool ready(void) const;
	private:
		IORange * ranges;
		int count;
		int cursor;
};

/****************************************************/
/**
 * Specialized version of a task to be used to perform IO operations.
 * The goal is to order the tasks in schedule groupes (layers).
 * We have the given assertions:
 *  - Tasks in the same schedule group can be executed in parallel.
 *  - A taks also point to one of the task in the next schedule group.
 *  - On termination we loop on all the task in the next schedule group
 *    and decrement the dependency counter of each task colliding with
 *    the current one. If it fall to 0, then the task can be started.
**/
class TaskIO : public Task
{
	public:
		TaskIO(TaksIOType ioType, const IORanges & ioRanges);
		virtual ~TaskIO(void) {};
		bool isActive(void) const;
		void activate(void);
		void registerToUnblock(TaskIO * task);
		std::deque<TaskIO*> & getBlockedTasks(void);
		bool canRunInParallel(const TaskIO * task) const;
		bool collide(const TaskIO * task) const {return this->ioRanges.collide(task->ioRanges);};
		bool isBlocked(void) const {return this->blockingDependencies > 0;};
		bool unblock(void);
	private:
		static inline bool oneIs(const TaskIO * task1, const TaskIO * task2, TaksIOType type);
		static inline bool oneOrTheOtherIs(const TaskIO * task1, const TaskIO * task2, TaksIOType type1, TaksIOType type2);
		static inline bool both(const TaskIO * task1, const TaskIO * task2, TaksIOType type);
	private:
		/** List of task to unblock when this one finishes. **/
		std::deque<TaskIO*> toUnblock;
		/** Object range to which the IO applies. **/
		IORanges ioRanges;
		/** Count blocking dependencies to know when we can start the task. **/
		int blockingDependencies;
		/** Define the type of IO. **/
		TaksIOType ioType;
		/** Is in active list. **/
		bool active;
};

/****************************************************/
inline IORange::IORange(size_t address, size_t size)
{
	this->address = address;
	this->size = size; 
};

/****************************************************/
inline bool IORange::collide(const IORange & range) const
{
	return ! (range.end() <= this->address || range.address >= this->end()); 
};

/****************************************************/
inline size_t IORange::end(void) const
{
	return this->address + this->size;
};

}

#endif //IOC_TASK_IO_HPP