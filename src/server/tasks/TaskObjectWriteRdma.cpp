/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <cassert>
#include <utility>
#include "TaskObjectWriteRdma.hpp"
#include "base/network/Protocol.hpp"
#include "../core/Consts.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * @todo optimisze not using SIZE_MAX if the flush range is smaller !
**/
TaskObjectWriteRdma::TaskObjectWriteRdma(LibfabricConnection * connection, LibfabricClientRequest & request, Container * container, ServerStats * stats, LibfabricObjReadWriteInfos objReadWrite, ObjectAccessMode mode)
                :TaskDeferredOps((mode==ACCESS_WRITE?IO_TYPE_WRITE:IO_TYPE_READ), ObjectRange(objReadWrite.objectId, objReadWrite.offset, objReadWrite.size))
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
	this->mode = mode;
}

/****************************************************/
void TaskObjectWriteRdma::runPrepare(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:write:rdma", "%1.runPrepare(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//get buffers from object
	Object & object = this->container->getObject(objReadWrite.objectId);
	if (this->mode == ACCESS_WRITE)
		this->status = object.getBuffers(this->ops, this->segments, objReadWrite.offset, objReadWrite.size, ACCESS_WRITE, true, true);
	else if (this->mode == ACCESS_READ)
		this->status = object.getBuffers(this->ops, this->segments, objReadWrite.offset, objReadWrite.size, ACCESS_READ, true, false);
	else
		IOC_FATAL_ARG("Invalid access mode to get buffers : %1").arg(this->mode).end();
}

/****************************************************/
void TaskObjectWriteRdma::runPostAction(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:write:rdma", "%1.runPostAction(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//check what to do
	if (this->status && this->ret >= 0) {
		this->setDetachedPost();
		this->performRdmaOps();
		if (this->mode == ACCESS_WRITE) {
			Object & object = this->container->getObject(objReadWrite.objectId);
			object.markDirty(objReadWrite.offset, objReadWrite.size);
		}
	} else {
		connection->sendResponse(IOC_LF_MSG_OBJ_READ_WRITE_ACK, request.lfClientId, -1);
	}
}

/****************************************************/
/**
 * @todo handle thread safety on stat increment
**/
void TaskObjectWriteRdma::performRdmaOps(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:write:rdma", "%1.performRdmaOps(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

	//build iovec
	iovec * iov = Object::buildIovec(segments, objReadWrite.offset, objReadWrite.size);
	size_t size = objReadWrite.size;

	//count number of ops
	int  * ops = new int;
	*ops = 0;
	for (size_t i = 0 ; i < segments.size() ; i += IOC_LF_MAX_RDMA_SEGS)
		(*ops)++;

	//loop on all send groups (because LF cannot send more than 256 at same time)
	size_t offset = 0;
	for (size_t i = 0 ; i < segments.size() ; i += IOC_LF_MAX_RDMA_SEGS) {
		//calc cnt
		size_t cnt = segments.size() - i;
		if (cnt > IOC_LF_MAX_RDMA_SEGS)
			cnt = IOC_LF_MAX_RDMA_SEGS;

		//emit rdma write vec & implement callback
		LibfabricAddr addr = objReadWrite.iov.addr + offset;
		uint64_t key = objReadWrite.iov.key;
		this->rdmaOpv(request.lfClientId, iov + i, cnt, addr, key, [ops, size, this](void){
			//decrement
			(*ops)--;

			//debug
			IOC_DEBUG_ARG("task:obj:write:rdma", "%1.finishRdmaOp(counter=%2)").arg(this).arg(ops).end();

			if (*ops == 0) {
				//stats
				this->stats->writeSize += size;

				//debug
				IOC_DEBUG_ARG("task:obj:write:rdma", "%1.sendResponse(%2)").arg(this).arg(Serializer::stringify(objReadWrite)).end();

				//send response
				this->connection->sendResponse(IOC_LF_MSG_OBJ_READ_WRITE_ACK, request.lfClientId, 0);

				//clean
				delete ops;

				//make the task terminated
				this->terminateDetachedPost();
			}
			
			return LF_WAIT_LOOP_KEEP_WAITING;
		});

		//update offset
		for (size_t j = 0 ; j < cnt ; j++)
			offset += iov[i+j].iov_len;
	}

	//remove temp
	delete [] iov;
}

/****************************************************/
void TaskObjectWriteRdma::rdmaOpv(int destinationEpId, struct iovec * iov, int count, LibfabricAddr remoteAddr, uint64_t remoteKey, std::function<LibfabricActionResult(void)> postAction)
{
	//REMARK, for a write op request, we perfrom a rdmaRead to fetch the data from the client to the server.
	//Invert for the read op.
	if (this->mode == ACCESS_READ) {
		this->connection->rdmaWritev(destinationEpId, iov, count, remoteAddr, remoteKey, postAction);
	} else if (this->mode == ACCESS_WRITE) {
		this->connection->rdmaReadv(destinationEpId, iov, count, remoteAddr, remoteKey, postAction);
	} else {
		IOC_FATAL_ARG("Invalid access mode to perform RDMA operation : %1")
			.arg(this->mode)
			.end();
	}
}
