/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
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
		TaskDeferredOps(TaksIOType ioType, const ObjectRange & objectRange, DeferredOperationList & ops);
		virtual void runAction(void) override;
		virtual void runPostAction(void) override;
	protected:
		DeferredOperationList ops;
		ssize_t ret;
};

}

#endif //IOC_TASK_DEFERRED_OPS_HPP
