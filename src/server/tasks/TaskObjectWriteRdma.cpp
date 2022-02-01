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
TaskObjectWriteRdma::TaskObjectWriteRdma(LibfabricConnection * connection, LibfabricClientRequest & request, Container * container, ServerStats * stats, LibfabricObjReadWriteInfos objReadWrite)
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
void TaskObjectWriteRdma::runPrepare(void)
{
	//get buffers from object
	Object & object = this->container->getObject(objReadWrite.objectId);
	this->status = object.getBuffers(this->ops, this->segments, objReadWrite.offset, objReadWrite.size, ACCESS_WRITE, true, true);
}

/****************************************************/
void TaskObjectWriteRdma::runPostAction(void)
{
	if (this->status && this->ret >= 0) {
		this->setDetachedPost();
		this->performRdmaOps();
		Object & object = this->container->getObject(objReadWrite.objectId);
		object.markDirty(objReadWrite.offset, objReadWrite.size);
	} else {
		connection->sendResponse(IOC_LF_MSG_OBJ_READ_WRITE_ACK, request.lfClientId, 0);
	}
}

/****************************************************/
void TaskObjectWriteRdma::performRdmaOps(void)
{
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
		connection->rdmaReadv(request.lfClientId, iov + i, cnt, addr, key, [ops, size, this](void){
			//decrement
			(*ops)--;

			if (*ops == 0) {
				//stats
				this->stats->writeSize += size;

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
