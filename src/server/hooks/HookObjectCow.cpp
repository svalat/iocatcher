/*****************************************************
			PROJECT  : IO Catcher
			LICENSE  : Apache 2.0
			COPYRIGHT: 2020 Bull SAS
*****************************************************/

/****************************************************/
#include "base/common/Debug.hpp"
#include "base/network/LibfabricConnection.hpp"
#include "HookObjectCow.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the object create hook.
 * @param container The container to be able to access objects to create.
**/
HookObjectCow::HookObjectCow(Container * container)
{
	this->container = container;
}

/****************************************************/
LibfabricActionResult HookObjectCow::onMessage(LibfabricConnection * connection, LibfabricClientMessage & message)
{
	//extract
	LibfabricObjectCow & objCow = message.message->data.objCow;

	//debug
	IOC_DEBUG_ARG("hook:obj:cow", "Get copy on write from %1 to %2 from client %3")
		.arg(objCow.sourceObjectId)
		.arg(objCow.destObjectId)
		.arg(message.lfClientId)
		.end();
	
	//extract id
	LibfabricObjectId & sourceId = objCow.sourceObjectId;
	LibfabricObjectId & destId = objCow.destObjectId;;

	//create object
	bool status;
	LibfabricObjectCow & objectCow = objCow;
	if (objectCow.rangeSize == 0)
		status = this->container->makeObjectFullCow(sourceId, destId, objectCow.allowExist);
	else
		status = this->container->makeObjectRangeCow(sourceId, destId, objectCow.allowExist, objectCow.rangeOffset, objectCow.rangeSize); 

	//send response
	connection->sendResponse(IOC_LF_MSG_OBJ_COW_ACK, message.lfClientId, (status)?0:-1);

	//republish
	connection->repostReceive(message.msgBufferId);

	//ret
	return LF_WAIT_LOOP_KEEP_WAITING;
}
