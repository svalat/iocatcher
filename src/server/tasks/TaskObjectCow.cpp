/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <cassert>
#include <utility>
#include "TaskObjectCow.hpp"
#include "base/network/Protocol.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * @todo optimisze not using SIZE_MAX if the flush range is smaller !
**/
TaskObjectCow::TaskObjectCow(LibfabricConnection * connection, LibfabricClientRequest & request, Container * container, LibfabricObjectCow objCow)
                :TaskIO(IO_TYPE_WRITE, 2)
                ,request(request)
                ,objCow(objCow)
{
	//check
	assert(connection != NULL);

	//calc the range
	size_t offset = objCow.rangeOffset;
	size_t size = objCow.rangeSize;
	if (size == 0)
		size = SIZE_MAX;

	//build dependency on objects to scehdule
	this->pushObjectRange(ObjectRange(objCow.sourceObjectId, offset, size));
	this->pushObjectRange(ObjectRange(objCow.destObjectId, offset, size));

	//set
	this->connection = connection;
	this->container = container;
	this->res = -1;
}

/****************************************************/
void TaskObjectCow::runPrepare(void)
{
	//flush object
	Object & srcObject = this->container->getObject(this->objCow.sourceObjectId);
	Object & destObject = this->container->getObject(this->objCow.destObjectId);

	//calc the range
	size_t offset = objCow.rangeOffset;
	size_t size = objCow.rangeSize;
	if (size == 0)
		size = SIZE_MAX;

	//extract ranges
	IORanges srcRanges = srcObject.getMemRanges(offset, size);
	IORanges destRanges = destObject.getMemRanges(offset, size);

	//build final
	IORanges ranges(srcRanges.getCursor() + destRanges.getCursor());
	this->setMemRanges(std::move(ranges));
}

/****************************************************/
void TaskObjectCow::runAction(void)
{
	//create object
	bool status;
	LibfabricObjectCow & objectCow = objCow;
	ObjectId sourceId = objCow.sourceObjectId;
	ObjectId destId = objCow.destObjectId;

	//apply
	if (objectCow.rangeSize == 0)
		status = this->container->makeObjectFullCow(sourceId, destId, objectCow.allowExist);
	else
		status = this->container->makeObjectRangeCow(sourceId, destId, objectCow.allowExist, objectCow.rangeOffset, objectCow.rangeSize); 

	//send response
	this->res = ((status)?0:-1);
}


/****************************************************/
void TaskObjectCow::runPostAction(void)
{
	//send response
	connection->sendResponse(IOC_LF_MSG_OBJ_COW_ACK, request.lfClientId, res);

	//republish
	request.terminate();
}