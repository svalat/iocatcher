/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_TASK_OBJECT_WRITE_EAGER_HPP
#define IOC_TASK_OBJECT_WRITE_EAGER_HPP

/****************************************************/
#include "../../base/network/LibfabricConnection.hpp"
#include "../core/DeferredOperation.hpp"
#include "TaskDeferredOps.hpp"
#include "../core/Container.hpp"
#include "../core/ServerStats.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Implement the server side handling of ping-pong operations.
**/
class TaskObjectWriteEager : public TaskDeferredOps
{
	public:
		TaskObjectWriteEager(LibfabricConnection * connection, LibfabricClientRequest & request, Container * container, ServerStats * stats, LibfabricObjReadWriteInfos objReadWrite);
	protected:
		virtual void runPostAction(void) override;
		virtual void runAction(void) override;
		virtual void runPrepare(void) override;
	private:
		void performMemcpyOps(void);
	private:
		LibfabricConnection * connection;
		LibfabricClientRequest request;
		Container * container;
		LibfabricObjReadWriteInfos objReadWrite;
		bool status;
		ObjectSegmentList segments;
		ServerStats * stats;
};

}

#endif //IOC_TASK_OBJECT_WRITE_EAGER_HPP
