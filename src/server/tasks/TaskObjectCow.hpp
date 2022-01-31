/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_TASK_OBJECT_COW_HPP
#define IOC_TASK_OBJECT_COW_HPP

/****************************************************/
#include "../../base/network/LibfabricConnection.hpp"
#include "../core/DeferredOperation.hpp"
#include "TaskDeferredOps.hpp"
#include "../core/Container.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Implement the server side handling of ping-pong operations.
**/
class TaskObjectCow : public TaskIO
{
	public:
		TaskObjectCow(LibfabricConnection * connection, LibfabricClientRequest & request, Container * container, LibfabricObjectCow objCow);
	protected:
		virtual void runPostAction(void) override;
		virtual void runAction(void) override;
		virtual void runPrepare(void) override;
	private:
		LibfabricConnection * connection;
		LibfabricClientRequest request;
		Container * container;
		LibfabricObjectCow objCow;
		int32_t res;
};

}

#endif //IOC_TASK_OBJECT_COW_HPP
