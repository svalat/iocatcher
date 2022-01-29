/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

#ifndef IOC_TASK_HPP
#define IOC_TASK_HPP

/****************************************************/
#include "base/common/Debug.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
enum TaskStage
{
	STAGE_PREPARE,
	STAGE_ACTION,
	STAGE_POST,
	STAGE_FINISHED
};

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
		Task(void) {this->immediate = false; this->nextStage = STAGE_PREPARE;};
		virtual ~Task(void) {};
		/** Run next stage. **/
		inline bool runNextStage(void);
		/** Run next stage. **/
		inline bool runNextStage(TaskStage expectedStage);
		/** Perform both actions in one go. **/
		inline void performAll(void);
		/** Tell to the worker manager to run the task immediatly. **/
		bool isImmediate(void) const {return this->immediate;};
		/** Mark as immediate. **/
		void markAsImmediate(void) {this->immediate = true;};
		bool getStage(void) const {return this->nextStage;};
	protected:
		/** Prepage stage to compute the buffer addresses for the run stage. */
		virtual void runPrepare(void) = 0;
		/** Perform the given action in the worker thread. */
		virtual void runAction(void) = 0;
		/** 
		 * To be called in the network handling thread after the action has 
		 * been performed by the worker. 
		**/
		virtual void runPostAction(void) = 0;
	private:
		/** Define if the task need to be runned immediately of not. **/
		bool immediate;
		/** Next stage to perform. **/
		TaskStage nextStage;
};

/****************************************************/
bool Task::runNextStage(void)
{
	//var
	bool res = false;

	//apply
	switch(this->nextStage) {
		case STAGE_PREPARE:
			this->runPrepare();
			this->nextStage = STAGE_ACTION;
			break;
		case STAGE_ACTION:
			this->runAction();
			this->nextStage = STAGE_POST;
			break;
		case STAGE_POST:
			this->runPostAction();
			this->nextStage = STAGE_FINISHED;
			res = true;
			break;
		default:
			IOC_FATAL("Invalid stage to run !");
			break;
	}

	//ret
	return res;
}

/****************************************************/
bool Task::runNextStage(TaskStage expectedStage)
{
	//check
	assumeArg(this->nextStage == expectedStage, "Invalid stage (%1), expected (%2)")
		.arg(this->nextStage)
		.arg(expectedStage)
		.end();

	//run
	return this->runNextStage();
}

/****************************************************/
void Task::performAll(void)
{
	while(this->runNextStage()) {};
}

}

#endif //IOC_TASK_HPP
