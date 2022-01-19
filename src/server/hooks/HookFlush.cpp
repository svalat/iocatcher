/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
#include "base/common/Debug.hpp"
#include "base/network/LibfabricConnection.hpp"
#include "../tasks/TaskObjectFlush.hpp"
#include "HookFlush.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the flush hook.
 * @param container The container to be able to access objects to flush.
**/
HookFlush::HookFlush(Container * container, TaskScheduler * taskScheduler, WorkerManager * workflowManager)
{
	//check
	assert(container != NULL);
	assert(taskScheduler != NULL);
	assert(workflowManager != NULL);

	//set
	this->container = container;
	this->taskScheduler = taskScheduler;
	this->workerManager = workflowManager;
}

/****************************************************/
LibfabricActionResult HookFlush::onMessage(LibfabricConnection * connection, LibfabricClientRequest & request)
{
	//extract
	LibfabricObjFlushInfos objFlush;
	request.deserializer.apply("objFlush", objFlush);

	//debug
	IOC_DEBUG_ARG("hook:obj:flush", "Get flush %1 client %2")
		.arg(Serializer::stringify(objFlush))
		.arg(request.lfClientId)
		.end();

	//flush object
	Object & object = this->container->getObject(objFlush.objectId);
	DeferredOperationList ops;
	object.flush(ops, objFlush.offset, objFlush.size);

	//build task to delegate to a worker thread
	TaskIO * task = new TaskObjectFlush(connection, request, ops);
	if (this->taskScheduler->pushTask(task))
		this->workerManager->pushTask(task);

	//ok
	return LF_WAIT_LOOP_KEEP_WAITING;
}
