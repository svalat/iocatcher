/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
//std
#include <cstring>
#include <cassert>
//local
#include "base/common/Debug.hpp"
#include "base/network/LibfabricConnection.hpp"
#include "HookObjectRead.hpp"
#include "../tasks/TaskObjectReadWriteRdma.hpp"
#include "../tasks/TaskObjectReadEager.hpp"
#include "../core/Consts.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the object read hook.
 * @param container The container to be able to access objects to with read operation.
**/
HookObjectRead::HookObjectRead(Container * container, ServerStats * stats, TaskRunner * taskRunner)
{
	//check
	assert(container != NULL);
	assert(stats != NULL);
	assert(taskRunner != NULL);

	//assign
	this->container = container;
	this->stats = stats;
	this->taskRunner = taskRunner;
}

/****************************************************/
LibfabricActionResult HookObjectRead::onMessage(LibfabricConnection * connection, LibfabricClientRequest & request)
{
	//extract
	LibfabricObjReadWriteInfos objReadWrite;
	request.deserializer.apply("objReadWrite", objReadWrite);

	//debug
	IOC_DEBUG_ARG("hook:obj:read", "Get object read %1 from client %2")
		.arg(Serializer::stringify(objReadWrite))
		.arg(request.lfClientId)
		.end();

	//check case
	if (objReadWrite.size <= IOC_EAGER_MAX_READ) {
		//launch the task
		IOTask * task = new TaskObjectReadEager(connection, request, container, stats, objReadWrite);
		this->taskRunner->pushTask(task);
	} else {
		//launch the task
		IOTask * task = new TaskObjectReadWriteRdma(connection, request, container, stats, objReadWrite, ACCESS_READ);
		this->taskRunner->pushTask(task);
	}

	//we do not need the request anymore (no data in)
	request.terminate();

	return LF_WAIT_LOOP_KEEP_WAITING;
}
