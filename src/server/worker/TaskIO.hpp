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
	inline IORange(size_t offset, size_t size);
	inline bool collide(const IORange & range) const;
	inline size_t end(void) const;
	size_t offset;
	size_t size;
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
class TaskIO
{
	public:
		TaskIO(TaksIOType ioType, IORange ioRange);
		virtual ~TaskIO(void) {};
		bool isActive(void) const {return this->active;};
		void activate(void) {this->active = true;};
		void registerInScheduleHierarchy(TaskIO * task);
		bool canRunInParallel(const TaskIO * task) const;
		bool collide(const TaskIO * task) const {return this->ioRange.collide(task->ioRange);};
	private:
		static inline bool oneIs(const TaskIO * task1, const TaskIO * task2, TaksIOType type);
		static inline bool oneOrTheOtherIs(const TaskIO * task1, const TaskIO * task2, TaksIOType type1, TaksIOType type2);
		static inline bool both(const TaskIO * task1, const TaskIO * task2, TaksIOType type);
	private:
		/** Task to unlock when it finishes. It can be one of the schedule groupe to execute next. **/
		TaskIO * toUnlock;
		/** Next task in same schedule group (which can be launched in parallel). It forms a circular single linked list. **/
		TaskIO * schedGroupNext;
		/** Object range to which the IO applies. **/
		IORange ioRange;
		/** Count the number of tasks in the previous schedule group which block the current one. **/
		size_t blockingDependencies;
		/** Define the type of IO. **/
		TaksIOType ioType;
		/** Is in active list. **/
		bool active;
};

/****************************************************/
inline IORange::IORange(size_t offset, size_t size)
{
	this->offset = offset;
	this->size = size; 
};

/****************************************************/
inline bool IORange::collide(const IORange & range) const
{
	return ! (range.end() <= this->offset || range.offset >= this->end()); 
};

/****************************************************/
inline size_t IORange::end(void) const
{
	return this->offset + this->size;
};

}

#endif //IOC_TASK_IO_HPP
