/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
#include "base/common/Debug.hpp"
#include "base/network/LibfabricConnection.hpp"
#include "../tasks/TaskObjectCow.hpp"
#include "HookObjectCow.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the object create hook.
 * @param container The container to be able to access objects to create.
**/
HookObjectCow::HookObjectCow(Container * container, TaskRunner * taskRunner)
{
	assert(container != NULL);
	assert(taskRunner != NULL);
	this->container = container;
	this->taskRunner = taskRunner;
}

/****************************************************/
LibfabricActionResult HookObjectCow::onMessage(LibfabricConnection * connection, LibfabricClientRequest & request)
{
	//extract
	LibfabricObjectCow objCow;
	request.deserializer.apply("objCow", objCow);

	//debug
	IOC_DEBUG_ARG("hook:obj:cow", "Get copy on write %1 from client %2")
		.arg(Serializer::stringify(objCow))
		.arg(request.lfClientId)
		.end();

	//build task
	TaskObjectCow * taskCow = new TaskObjectCow(connection, request, container, objCow);

	//schedule the task
	this->taskRunner->pushTask(taskCow);

	//ret
	return LF_WAIT_LOOP_KEEP_WAITING;
}
