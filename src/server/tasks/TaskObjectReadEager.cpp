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
 * Constructor of a read eager task.
 * @param connection Define the connection to respond to the client.
 * @param lfClientId Define the client ID to which we want to respond.
 * @param container Define the container to be used to find the object.
 * @param stats Keep track of the transfer statistics.
 * @param objReadWrite Request informations.
**/
TaskObjectReadEager::TaskObjectReadEager(LibfabricConnection * connection, uint64_t lfClientId, Container * container, ServerStats * stats, LibfabricObjReadWriteInfos objReadWrite)
                :TaskDeferredOps(IO_TYPE_READ, ObjectRange(objReadWrite.objectId, objReadWrite.offset, objReadWrite.size))
                ,objReadWrite(objReadWrite)
                ,preBuiltResponse(IOC_LF_MSG_OBJ_READ_WRITE_ACK, lfClientId, connection)
{
	//check
	assert(connection != NULL);
	assert(stats != NULL);
	assert(container != NULL);

	//set
	this->container = container;
	this->stats = stats;
}

/****************************************************/
/**
 * For the prepare phase we look for the object and we call the getBuffer() function which
 * will allocate the new segments if required and build in this->ops the list of deferred operation
 * to perform data reading on new allocated segments. Those actions will be performed in the
 * runAction() phase.
 *
 * @todo Mark the task as immediate if the amound of data movement is small enougth.
**/
void TaskObjectReadEager::runPrepare(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:read:eager", "%1.runPrepare(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//get buffers from object
	Object & object = this->container->getObject(objReadWrite.objectId);
	this->status = object.getBuffers(this->ops, this->segments, objReadWrite.offset, objReadWrite.size, ACCESS_READ, true, false);

	//protect the memory ranges
	this->setMemRanges(this->ops.buildMemRanges());
}

/****************************************************/
/**
 * Apply the actions in the worker thread, meaning running the deferred operations to fetch
 * the data from the backend to the new allocated memory segments and to perform the transfers
 * to the response buffer by building the response.
**/
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
/**
 * As post action we respond to the client.
**/
void TaskObjectReadEager::runPostAction(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:read:eager", "%1.runPostAction(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//send
	this->preBuiltResponse.send();
}

/****************************************************/
/**
 * @todo handle thread safety on stat increment (atomic)
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
