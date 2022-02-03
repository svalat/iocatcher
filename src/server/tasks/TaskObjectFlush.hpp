/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_TASK_OBJECT_FLUSH_HPP
#define IOC_TASK_OBJECT_FLUSH_HPP

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
 * Taskify the flush operation to be performed in a worker thread.
**/
class TaskObjectFlush : public TaskDeferredOps
{
	public:
		TaskObjectFlush(LibfabricConnection * connection, uint64_t lfClientId, Container * container, LibfabricObjFlushInfos flushInfos);
	protected:
		virtual void runPostAction(void) override;
		virtual void runPrepare(void) override;
	private:
		/** Keep track of the connection to be used to send the response. **/
		LibfabricConnection * connection;
		/** Define the client waiting the response. **/
		uint64_t lfClientId;
		/** To find the required object during the prepare phase. **/
		Container * container;
		/** Contain the client request informations. **/
		LibfabricObjFlushInfos flushInfos;
};

}

#endif //IOC_TASK_OBJECT_FLUSH_HPP
