/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <cassert>
#include <utility>
#include "base/common/Debug.hpp"
#include "TaskDeferredOps.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the deferered operations.
 * @param ioType Defines the type of operation for the scheduling task parallelisation rules.
 * @param objectRange Define a single object range on which the operation applies and to be used
 * to detect task serialization. Here we consider the access to the memory segments which mean
 * that writing data to a file or to a remote client is a READ and loading data from a file
 * of fetching from client is a WRITE.
 * 
 * Notice the operation list can be (recommended) build int the runPrepare() phase to be sure
 * the object will not change on the buffer addresses point of view.
 * 
 * We build the operation in the runPrepare() phase because the object range protect the
 * possible modification of the object segment buffer addresses (COW, destaging) which
 * is guarantee (by the scheduler) to be protected when we enter in the runPrepare() phase.
 * 
 * Remark: the prepare phase should register the memory ranges to protect the access
 * to the buffers in case they are COW.
**/
TaskDeferredOps::TaskDeferredOps(IOTaksType ioType, const ObjectRange & objectRange)
                :IOTask(ioType, objectRange)
{
}

/****************************************************/
/**
 * Run the deferred operations and store the result in ret.
**/
void TaskDeferredOps::runAction(void)
{
	IOC_DEBUG_ARG("task:defops", "Run %1 operations").arg(this->ops.size()).end();
	this->ret = this->ops.runAll();
}

/****************************************************/
/**
 * Run the post action, currently do nothing.
**/
void TaskDeferredOps::runPostAction(void)
{
	//nothing to do
}

/****************************************************/
/**
 * Run a deferred list of operation which is already prepared.
**/
TaskDeferredOpsPrepared::TaskDeferredOpsPrepared(IOTaksType ioType, const ObjectRange & objectRange, DeferredOperationVector & ops)
                        :TaskDeferredOps(ioType, objectRange)
{
	//move into local ops
	this->ops = std::move(ops);

	//we can set the mem ranges now
	this->setMemRanges(this->ops.buildMemRanges());

	//determine if immediate
	if (this->ops.size() == 0)
		this->markAsImmediate();
}

/****************************************************/
/**
 * Already prepared so nothing to do.
**/
void TaskDeferredOpsPrepared::runPrepare(void)
{
	//do nothing
}
