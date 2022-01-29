/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
#include <cassert>
#include <utility>
#include "base/common/Debug.hpp"
#include "TaskDeferredOps.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
TaskDeferredOps::TaskDeferredOps(TaksIOType ioType, const ObjectRange & objectRange)
                :TaskIO(ioType, objectRange)
{
}

/****************************************************/
void TaskDeferredOps::runAction(void)
{
	IOC_DEBUG_ARG("task:defops", "Run %1 operations").arg(this->ops.size()).end();
	this->ret = this->ops.runAll();
}

/****************************************************/
void TaskDeferredOps::runPostAction(void)
{
	//nothing to do
}

/****************************************************/
TaskDeferredOpsPrepared::TaskDeferredOpsPrepared(TaksIOType ioType, const ObjectRange & objectRange, DeferredOperationList & ops)
                        :TaskDeferredOps(ioType, objectRange)
{
	//move into local ops
	this->ops = std::move(ops);

	//determine if immediate
	if (this->ops.size() == 0)
		this->markAsImmediate();
}

/****************************************************/
void TaskDeferredOpsPrepared::runPrepare(void)
{
	//nothing to do
}
