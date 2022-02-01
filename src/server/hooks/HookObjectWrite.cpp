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
#include "HookObjectWrite.hpp"
#include "../core/Consts.hpp"
#include "../tasks/TaskObjectWriteRdma.hpp"
#include "../tasks/TaskObjectWriteEager.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the object write hook.
 * @param container The container to be able to access objects to with write operation.
**/
HookObjectWrite::HookObjectWrite(Container * container, ServerStats * stats, TaskRunner * taskRunner)
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
LibfabricActionResult HookObjectWrite::onMessage(LibfabricConnection * connection, LibfabricClientRequest & request)
{
	//extract
	LibfabricObjReadWriteInfos objReadWrite;
	request.deserializer.apply("objReadWrite", objReadWrite);

	//debug
	IOC_DEBUG_ARG("hook:obj:write", "Get object write %1 from client %2")
		.arg(Serializer::stringify(objReadWrite))
		.arg(request.lfClientId)
		.end();

	//get buffers from object
	if (objReadWrite.msgHasData) {
		//launch the task
		TaskIO * task = new TaskObjectWriteEager(connection, request, container, stats, objReadWrite);
		this->taskRunner->pushTask(task);
	} else {
		//launch the task
		TaskIO * task = new TaskObjectWriteRdma(connection, request, container, stats, objReadWrite);
		this->taskRunner->pushTask(task);

		//we do not need the request anymore (data via RDMA)
		request.terminate();
	}

	return LF_WAIT_LOOP_KEEP_WAITING;
}
