/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_TASK_DEFERRED_OPS_HPP
#define IOC_TASK_DEFERRED_OPS_HPP

/****************************************************/
#include "../../base/network/LibfabricConnection.hpp"
#include "../core/DeferredOperation.hpp"
#include "../worker/IOTask.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Basic task to run deferred operations to be performed in the worker threads.
**/
class TaskDeferredOps : public IOTask
{
	public:
		TaskDeferredOps(IOTaksType ioType, const ObjectRange & objectRange);
	protected:
		virtual void runAction(void) override;
		virtual void runPostAction(void) override;
	protected:
		/** List of operations to be performed. **/
		DeferredOperationVector ops;
		/**
		 * Result containing either the sum of the operation sizes or 
		 * -1 if we encountered an error. 
		**/
		ssize_t ret;
};

/****************************************************/
/**
 * Basic task to run operations already prepared meaning we already know the
 * addresses on which it will apply and we are sure we do not change them.
**/
class TaskDeferredOpsPrepared : public TaskDeferredOps
{
	public:
		TaskDeferredOpsPrepared(IOTaksType ioType, const ObjectRange & objectRange, DeferredOperationVector & ops);
	protected:
		virtual void runPrepare(void);
};

}

#endif //IOC_TASK_DEFERRED_OPS_HPP
