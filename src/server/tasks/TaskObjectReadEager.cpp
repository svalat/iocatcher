/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <cassert>
#include <utility>
#include "TaskObjectReadEager.hpp"
#include "base/network/Protocol.hpp"
#include "../core/Consts.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * @todo optimisze not using SIZE_MAX if the flush range is smaller !
**/
TaskObjectReadEager::TaskObjectReadEager(LibfabricConnection * connection, LibfabricClientRequest & request, Container * container, ServerStats * stats, LibfabricObjReadWriteInfos objReadWrite)
                :TaskDeferredOps(IO_TYPE_READ, ObjectRange(objReadWrite.objectId, objReadWrite.offset, objReadWrite.size))
                ,request(request)
                ,objReadWrite(objReadWrite)
                ,preBuiltResponse(IOC_LF_MSG_OBJ_READ_WRITE_ACK, request.lfClientId, connection)
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
void TaskObjectReadEager::runPrepare(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:read:eager", "%1.runPrepare(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//get buffers from object
	Object & object = this->container->getObject(objReadWrite.objectId);
	this->status = object.getBuffers(this->ops, this->segments, objReadWrite.offset, objReadWrite.size, ACCESS_READ, true, false);
}

/****************************************************/
void TaskObjectReadEager::runAction(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:read:eager", "%1.runAction(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//perform pread ops
	TaskDeferredOps::runAction();

	//make memcpys
	this->performMemcpyOps();

	//apply success or not
	if (this->status && this->ret >= 0)
		this->preBuiltResponse.setStatus(objReadWrite.size);
	else
		this->preBuiltResponse.setStatus(-1);

	//pre built to make the memcpy to the send buffer in the worker thread
	this->preBuiltResponse.build();
}

/****************************************************/
void TaskObjectReadEager::runPostAction(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:read:eager", "%1.runPostAction(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//send
	this->preBuiltResponse.send();
}

/****************************************************/
/**
 * @todo handle thread safety on stat increment
**/
void TaskObjectReadEager::performMemcpyOps(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:read:eager", "%1.performMemcpyOps(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

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
	this->preBuiltResponse.setBuffers(buffers, segments.size(), dataSize);
}
