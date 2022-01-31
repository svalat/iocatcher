/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
#include <cassert>
#include <utility>
#include "TaskObjectFlush.hpp"
#include "base/network/Protocol.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * @todo optimisze not using SIZE_MAX if the flush range is smaller !
**/
TaskObjectFlush::TaskObjectFlush(LibfabricConnection * connection, LibfabricClientRequest & request, Container * container, LibfabricObjFlushInfos flushInfos)
                :TaskDeferredOps(IO_TYPE_READ, ObjectRange(flushInfos.objectId, 0, SIZE_MAX))
                ,request(request)
                ,flushInfos(flushInfos)
{
	//check
	assert(connection != NULL);

	//set
	this->connection = connection;
	this->container = container;
}

/****************************************************/
void TaskObjectFlush::runPostAction(void)
{
	//send response
	connection->sendResponse(IOC_LF_MSG_OBJ_FLUSH_ACK, request.lfClientId, ret);

	//republish
	request.terminate();
}

/****************************************************/
void TaskObjectFlush::runPrepare(void)
{
	//build flush operations
	Object & object = this->container->getObject(this->flushInfos.objectId);
	object.flush(this->ops, this->flushInfos.offset, this->flushInfos.size);

	//build me ranges for dependency checking
	//REMARK: we need to check the buffer addresses due to the COW support which might
	//change them. If we remove COW this line can be removed.
	this->setMemRanges(this->ops.buildMemRanges());
}
