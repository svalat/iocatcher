/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

#ifndef IOC_TASK_OBJECT_FLUSH_HPP
#define IOC_TASK_OBJECT_FLUSH_HPP

/****************************************************/
#include "../../base/network/LibfabricConnection.hpp"
#include "../core/DeferredOperation.hpp"
#include "../worker/TaskIO.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Implement the server side handling of ping-pong operations.
**/
class TaskObjectFlush : public TaskIO
{
	public:
		TaskObjectFlush(LibfabricConnection * connection, LibfabricClientRequest & request, DeferredOperationList & ops);
		virtual void runAction(void) override;
		virtual void runPostAction(void) override;
	private:
		LibfabricConnection * connection;
		LibfabricClientRequest request;
		DeferredOperationList ops;
		ssize_t ret;
};

}

#endif //IOC_TASK_OBJECT_FLUSH_HPP
