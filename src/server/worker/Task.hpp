/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

#ifndef IOC_TASK_HPP
#define IOC_TASK_HPP

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Define what is a task to be deferred to a worker thread. A task has two
 * actions. A run action to be called in the worker thread and a post-action
 * to be called on the main network handling thread when the task has been
 * performed.
**/
class Task
{
	public:
		Task(void) {};
		virtual ~Task(void) {};
		/** Perform the given action in the worker thread. */
		virtual void runAction(void) = 0;
		/** 
		 * To be called in the network handling thread after the action has 
		 * been performed by the worker. 
		**/
		virtual void runPostAction(void) = 0;
		/** Perform both actions in one go. **/
		void performBoth(void) {this->runAction(); this->runPostAction();};
};

}

#endif //IOC_TASK_HPP