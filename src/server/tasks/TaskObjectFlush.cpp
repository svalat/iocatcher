/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
#include <cassert>
#include <utility>
#include "TaskObjectFlush.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
TaskObjectFlush::TaskObjectFlush(LibfabricConnection * connection, LibfabricClientRequest & request, DeferredOperationList & ops)
                :TaskIO(IO_TYPE_READ, ops.buildIORanges())
                ,request(request)
                ,ops(std::move(ops))
{
	//check
	assert(connection != NULL);

	//set
	this->connection = connection;
}

/****************************************************/
void TaskObjectFlush::runAction(void)
{
	this->ret = this->ops.runAll();
}

/****************************************************/
void TaskObjectFlush::runPostAction(void)
{
	//send response
	connection->sendResponse(IOC_LF_MSG_OBJ_FLUSH_ACK, request.lfClientId, ret);

	//republish
	request.terminate();
}
