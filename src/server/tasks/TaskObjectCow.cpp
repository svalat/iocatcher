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
 * Constructor of the object task COW.
 * @param connection Define the connection to be used to send back the response.
 * @param lfClientId Define to who to respond.
 * @param container Define the container to be used to find the object.
 * @param objCow The deserialized client request informations.
**/
TaskObjectCow::TaskObjectCow(LibfabricConnection * connection, uint64_t lfClientId, Container * container, LibfabricObjectCow objCow)
                :IOTask(IO_TYPE_WRITE, 2)
                ,objCow(objCow)
{
	//check
	assert(connection != NULL);

	//calc the range
	size_t offset = objCow.rangeOffset;
	size_t size = objCow.rangeSize;
	if (size == 0)
		size = SIZE_MAX;

	//build dependency on the two object we interact with
	this->pushObjectRange(ObjectRange(objCow.sourceObjectId, offset, size));
	this->pushObjectRange(ObjectRange(objCow.destObjectId, offset, size));

	//set
	this->connection = connection;
	this->container = container;
	this->res = -1;
	this->lfClientId = lfClientId;

	//mark as immediate task to run in the core thread
	this->markAsImmediate();
}

/****************************************************/
/**
 * Prepare the opration by computing the memory buffers we will apply on.
**/
void TaskObjectCow::runPrepare(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:cow", "%1.runPrepare(%2)").arg(this).arg(Serializer::stringify(objCow)).end();

	//flush object
	Object & srcObject = this->container->getObject(this->objCow.sourceObjectId);
	Object & destObject = this->container->getObject(this->objCow.destObjectId);

	//calc the range
	size_t offset = objCow.rangeOffset;
	size_t size = objCow.rangeSize;
	if (size == 0)
		size = SIZE_MAX;

	//extract ranges
	MemRanges srcRanges = srcObject.getMemRanges(offset, size);
	MemRanges destRanges = destObject.getMemRanges(offset, size);

	//build final
	MemRanges ranges(srcRanges.getCursor() + destRanges.getCursor());
	this->setMemRanges(std::move(ranges));
}

/****************************************************/
/**
 * Perform the action.
 * 
 * Remark: this one should be sure to run on the networking thread because we
 * access the objects and change their state. This is guarantied becuse we
 * called markAsImmediate() in the constructor.
**/
void TaskObjectCow::runAction(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:cow", "%1.runAction(%2)").arg(this).arg(Serializer::stringify(objCow)).end();

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
/**
 * As a post action when done we can send the reponse to the client to tell
 * the COW has been done.
**/
void TaskObjectCow::runPostAction(void)
{
	//debug
	IOC_DEBUG_ARG("task:obj:cow", "%1.runPostAction(%2)").arg(this).arg(Serializer::stringify(objCow)).end();

	//send response
	connection->sendResponse(IOC_LF_MSG_OBJ_COW_ACK, this->lfClientId, res);
}
