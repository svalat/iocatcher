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
 * Implement the tasking for the COW operation to be created in the HookObjectCow.
**/
class TaskObjectCow : public IOTask
{
	public:
		TaskObjectCow(LibfabricConnection * connection, uint64_t lfClientId, Container * container, LibfabricObjectCow objCow);
	protected:
		virtual void runPostAction(void) override;
		virtual void runAction(void) override;
		virtual void runPrepare(void) override;
	private:
		/** Keep track of the connection to know how to send the final response when done. **/
		LibfabricConnection * connection;
		/** Keep track of the client ID to respond to. **/
		uint64_t lfClientId;
		/** Keep track of the container to find the object we apply to. **/
		Container * container;
		/** Keep track of the deserialized cow message informations. **/
		LibfabricObjectCow objCow;
		/** Keep track of the final status to be returned by the response to the client. **/
		int32_t res;
};

}

#endif //IOC_TASK_OBJECT_COW_HPP
