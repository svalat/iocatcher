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
#include "IORanges.hpp"
#include "ObjectRanges.hpp"

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
		TaskIO(TaksIOType ioType,const IORanges & memRanges);
		TaskIO(TaksIOType ioType, const ObjectRange & objectRange, const IORanges & memRanges);
		virtual ~TaskIO(void) {};
		bool isActive(void) const;
		void activate(void);
		void registerToUnblock(TaskIO * task);
		std::deque<TaskIO*> & getBlockedTasks(void);
		bool canRunInParallel(const TaskIO * task) const;
		inline bool collide(const TaskIO * task) const;
		inline bool isBlocked(void) const;
		bool unblock(void);
	private:
		static inline bool oneIs(const TaskIO * task1, const TaskIO * task2, TaksIOType type);
		static inline bool oneOrTheOtherIs(const TaskIO * task1, const TaskIO * task2, TaksIOType type1, TaksIOType type2);
		static inline bool both(const TaskIO * task1, const TaskIO * task2, TaksIOType type);
	private:
		/** List of task to unblock when this one finishes. **/
		std::deque<TaskIO*> toUnblock;
		/** 
		 * Object range to which the IO applies.
		 * @todo we need a multi-range because we need to protect the memory buffer which can be shared
		 * between multiple objects. We can optimize and use a single range with object ID
		 * if we remove the copy-on-write feature.
		**/
		IORanges memRanges;
		/** Protect the listed ranges in objects. **/
		ObjectRanges objectRanges;
		/** Count blocking dependencies to know when we can start the task. **/
		int blockingDependencies;
		/** Define the type of IO. **/
		TaksIOType ioType;
		/** Is in active list. **/
		bool active;
};

/****************************************************/
bool TaskIO::collide(const TaskIO * task) const
{
	return this->objectRanges.collide(task->objectRanges) || this->memRanges.collide(task->memRanges);
};

/****************************************************/
bool TaskIO::isBlocked(void) const
{
	return this->blockingDependencies > 0;
}

}

#endif //IOC_TASK_IO_HPP
