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
#include "../worker/TaskIO.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Basic task to run operations
**/
class TaskDeferredOps : public TaskIO
{
	public:
		TaskDeferredOps(TaksIOType ioType, const ObjectRange & objectRange);
	protected:
		virtual void runAction(void) override;
		virtual void runPostAction(void) override;
	protected:
		DeferredOperationList ops;
		ssize_t ret;
};

/****************************************************/
/**
 * Basic task to run operations already prepared
**/
class TaskDeferredOpsPrepared : public TaskDeferredOps
{
	public:
		TaskDeferredOpsPrepared(TaksIOType ioType, const ObjectRange & objectRange, DeferredOperationList & ops);
	protected:
		virtual void runPrepare(void);
};

}

#endif //IOC_TASK_DEFERRED_OPS_HPP
