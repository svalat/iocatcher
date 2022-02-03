/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_IO_TASK_HPP
#define IOC_IO_TASK_HPP

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
enum IOTaksType
{
	/** Read IO has the property to be performed in parallel on the same segment. **/
	IO_TYPE_READ,
	/** Write IO are exclusive on a given segment and exclusite with READ. **/
	IO_TYPE_WRITE
};

/****************************************************/
class TaskRunner;

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
class IOTask : public Task
{
	public:
		IOTask(IOTaksType ioType, int objectRangesCount);
		IOTask(IOTaksType ioType, const ObjectRange & objectRange);
		IOTask(IOTaksType ioType, const ObjectRange & objectRange, const MemRanges & memRanges);
		virtual ~IOTask(void) {};
		bool isActive(void) const;
		void activate(void);
		void registerToUnblock(IOTask * task);
		std::deque<IOTask*> & getBlockedTasks(void);
		bool canRunInParallel(const IOTask * task) const;
		inline bool collide(const IOTask * task) const;
		inline bool isBlocked(void) const;
		bool unblock(void);
		void setDetachedPost(void);
		bool isDetachedPost(void) const;
		void setTaskRunner(TaskRunner * runner);
	protected:
		void terminateDetachedPost(void);
		void setMemRanges(MemRanges && memRanges);
		void pushObjectRange(const ObjectRange & objectRange);
	private:
		static inline bool oneIs(const IOTask * task1, const IOTask * task2, IOTaksType type);
		static inline bool oneOrTheOtherIs(const IOTask * task1, const IOTask * task2, IOTaksType type1, IOTaksType type2);
		static inline bool both(const IOTask * task1, const IOTask * task2, IOTaksType type);
	private:
		/** List of task to unblock when this one finishes. **/
		std::deque<IOTask*> toUnblock;
		/** 
		 * Object range to which the IO applies.
		 * @todo we need a multi-range because we need to protect the memory buffer which can be shared
		 * between multiple objects. We can optimize and use a single range with object ID
		 * if we remove the copy-on-write feature.
		**/
		MemRanges memRanges;
		/** Protect the listed ranges in objects. **/
		ObjectRanges objectRanges;
		/** Count blocking dependencies to know when we can start the task. **/
		int blockingDependencies;
		/** Define the type of IO. **/
		IOTaksType ioType;
		/** Is in active list. **/
		bool active;
		/** Permit to detach the post operation to perform libfabric RDMA ops **/
		bool detachPost;
		/** Keep track of the task runner to notify when the detached post action has finished. **/
		TaskRunner * taskRunner;
};

/****************************************************/
bool IOTask::collide(const IOTask * task) const
{
	return this->objectRanges.collide(task->objectRanges) || this->memRanges.collide(task->memRanges);
};

/****************************************************/
bool IOTask::isBlocked(void) const
{
	return this->blockingDependencies > 0;
}

}

#endif //IOC_IO_TASK_HPP
