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
/**
 * Push data to the client making an eager communication and adding the data after the response
 * to the client.
 * @param clientId the libfabric client ID to know the connection to be used.
 * @param objReadWrite Reference to the read request information.
 * @param segments The list of object segments to be sent.
**/
void HookObjectRead::objEagerPushToClient(LibfabricConnection * connection, uint64_t clientId, LibfabricObjReadWriteInfos & objReadWrite, ObjectSegmentList & segments)
{
	//size
	size_t dataSize = objReadWrite.size;
	size_t baseOffset = objReadWrite.offset;

	//build vector
	LibfabricBuffer * buffers = new LibfabricBuffer[segments.size()];

	//copy data
	size_t cur = 0;
	int i = 0;
	for (auto segment : segments) {
		//compute copy size to stay in data limits
		size_t copySize = segment.size;
		size_t offset = 0;
		if (baseOffset > segment.offset) {
			offset = baseOffset - segment.offset;
			copySize -= offset;
		}
		assert(copySize <= segment.size);
		if (cur + copySize > dataSize) {
			copySize = dataSize - cur;
		}

		//copy
		assert(offset < segment.size);
		assert(copySize <= segment.size);
		assert(copySize <= segment.size - offset);

		//setup
		buffers[i].buffer = segment.ptr + offset;
		buffers[i].size = copySize;

		//progress
		cur += copySize;
	}

	//stats
	this->stats->readSize += dataSize;

	//send ack message
	connection->sendResponse(IOC_LF_MSG_OBJ_READ_WRITE_ACK, clientId, 0, buffers, segments.size());

	//free mem
	delete [] buffers;
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

	//check cas
	if (objReadWrite.size <= IOC_EAGER_MAX_READ) {
		//get buffers from object
		Object & object = this->container->getObject(objReadWrite.objectId);
		ObjectSegmentList segments;
		bool status = object.getBuffers(segments, objReadWrite.offset, objReadWrite.size, ACCESS_READ);

		//eager or rdma
		if (status) {
			if (objReadWrite.size <= IOC_EAGER_MAX_READ) {
				this->objEagerPushToClient(connection, request.lfClientId, objReadWrite, segments);
			}
		} else {
			connection->sendResponse(IOC_LF_MSG_OBJ_READ_WRITE_ACK, request.lfClientId, -1);
		}

		//republish
		request.terminate();
	} else {
		//launch the task
		IOTask * task = new TaskObjectReadWriteRdma(connection, request, container, stats, objReadWrite, ACCESS_READ);
		this->taskRunner->pushTask(task);

		//we do not need the request anymore (data via RDMA)
		request.terminate();
	}

	return LF_WAIT_LOOP_KEEP_WAITING;
}
