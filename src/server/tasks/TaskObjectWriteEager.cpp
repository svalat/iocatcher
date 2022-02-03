/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <cassert>
#include <utility>
#include "TaskObjectWriteEager.hpp"
#include "base/network/Protocol.hpp"
#include "../core/Consts.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the eager write task.
 * @param connection Define the connection to be used to send the response.
 * @param request Define the request containing the data and to be temrinated when the operation has been performed.
 * @param container Define the container used to find the object.
 * @param stats Define the stats structure to track the bandwidth.
 * @param objReadWrite Define the request informations.
**/
TaskObjectWriteEager::TaskObjectWriteEager(LibfabricConnection * connection, LibfabricClientRequest & request, Container * container, ServerStats * stats, LibfabricObjReadWriteInfos objReadWrite)
                :TaskDeferredOps(IO_TYPE_WRITE, ObjectRange(objReadWrite.objectId, objReadWrite.offset, objReadWrite.size))
                ,request(request)
                ,objReadWrite(objReadWrite)
{
	//check
	assert(connection != NULL);
	assert(stats != NULL);
	assert(container != NULL);

	//set
	this->connection = connection;
	this->container = container;
	this->stats = stats;
}

/****************************************************/
/**
 * Prepare the operation by allocating the required new buffers if needed and
 * build the deferred fetch operations.
 * @todo If there is no fetch operations and a small amount of data to copy we should
 * mark the task as immediate.
**/
void TaskObjectWriteEager::runPrepare(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:write:eager", "%1.runPrepare(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//get buffers from object
	Object & object = this->container->getObject(objReadWrite.objectId);
	this->status = object.getBuffers(this->ops, this->segments, objReadWrite.offset, objReadWrite.size, ACCESS_WRITE, true, true);

	//protect the memory ranges
	this->setMemRanges(this->ops.buildMemRanges());
}

/****************************************************/
/**
 * Run the operations and perform the memory copies.
**/
void TaskObjectWriteEager::runAction(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:write:eager", "%1.runAction(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//perform pread ops
	TaskDeferredOps::runAction();

	//make memcpys
	this->performMemcpyOps();
}

/****************************************************/
/**
 * As post action we mark the object as dirty, respond to the client and close the
 * request.
**/
void TaskObjectWriteEager::runPostAction(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:write:eager", "%1.runPostAction(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//apply success or not
	if (this->status && this->ret >= 0) {
		//mark dirty
		Object & object = this->container->getObject(objReadWrite.objectId);
		object.markDirty(objReadWrite.offset, objReadWrite.size);

		//send response
		connection->sendResponse(IOC_LF_MSG_OBJ_READ_WRITE_ACK, request.lfClientId, 0);
	} else {
		connection->sendResponse(IOC_LF_MSG_OBJ_READ_WRITE_ACK, request.lfClientId, -1);
	}

	//can free the request
	request.terminate();
}

/****************************************************/
/**
 * Perform the memory copies.
 * @todo handle thread safety on stat increment
**/
void TaskObjectWriteEager::performMemcpyOps(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:write:eager", "%1.performMemcpyOps(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//get base pointer
	const char * data = objReadWrite.optionalData;
	assert(data != NULL);

	//copy data
	size_t cur = 0;
	size_t dataSize = objReadWrite.size;
	size_t baseOffset = objReadWrite.offset;
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
		memcpy(segment.ptr + offset, data + cur, copySize);

		//progress
		cur += copySize;
	}

	//stats
	this->stats->writeSize += cur;
}
