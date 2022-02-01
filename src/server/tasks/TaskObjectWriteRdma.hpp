/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_TASK_OBJECT_WRITE_RDMA_HPP
#define IOC_TASK_OBJECT_WRITE_RDMA_HPP

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
class TaskObjectWriteRdma : public TaskDeferredOps
{
	public:
		TaskObjectWriteRdma(LibfabricConnection * connection, LibfabricClientRequest & request, Container * container, ServerStats * stats, LibfabricObjReadWriteInfos objReadWrite);
	protected:
		virtual void runPostAction(void) override;
		virtual void runPrepare(void) override;
	private:
		void performRdmaOps(void);
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

#endif //IOC_TASK_OBJECT_WRITE_RDMA_HPP
