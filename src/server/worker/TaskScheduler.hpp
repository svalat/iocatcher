/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

#ifndef IOC_TASK_SCHEDULER_HPP
#define IOC_TASK_SCHEDULER_HPP

/****************************************************/
#include <list>
#include <vector>
#include "TaskIO.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
typedef std::vector<TaskIO*> TaskVecor;

/****************************************************/
/**
 * Class used to schedule the IO task and make waiting those colliding and
 * not being able to run at the same time than the active tasks.
 * The idea is to attach a task scheduler for each object.
**/
class TaskScheduler
{
	public:
		bool pushTask(TaskIO * task);
		void popFinishedTask(TaskVecor & toStart, TaskIO * task);
	private:
		void removeFromList(TaskIO * task);
		bool canSchedule(TaskIO * task);
	private:
		/**
		 * Use a double linked list to order the task by they arrival time.
		 * This is a very naive implementation because we will need to loop
		 * in the list to find the task to remove. To be optimized if we see
		 * it costs too much.
		 * The new tasks arriving are added at then end of the list and are
		 * poped when they terminate.
		**/
		std::list<TaskIO*> tasks;
};

}

#endif //IOC_TASK_SCHEDULER_HPP
