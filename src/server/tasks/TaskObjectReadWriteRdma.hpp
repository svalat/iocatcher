/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_TASK_OBJECT_READ_WRITE_RDMA_HPP
#define IOC_TASK_OBJECT_READ_WRITE_RDMA_HPP

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
class TaskObjectReadWriteRdma : public TaskDeferredOps
{
	public:
		TaskObjectReadWriteRdma(LibfabricConnection * connection, uint64_t lfClientId, Container * container, ServerStats * stats, LibfabricObjReadWriteInfos objReadWrite, ObjectAccessMode mode);
	protected:
		virtual void runPostAction(void) override;
		virtual void runPrepare(void) override;
	private:
		void performRdmaOps(void);
		void rdmaOpv(int destinationEpId, struct iovec * iov, int count, LibfabricAddr remoteAddr, uint64_t remoteKey, std::function<LibfabricActionResult(void)> postAction);
	private:
		LibfabricConnection * connection;
		uint64_t lfClientId;
		Container * container;
		LibfabricObjReadWriteInfos objReadWrite;
		bool status;
		ObjectSegmentList segments;
		ServerStats * stats;
		ObjectAccessMode mode;
};

}

#endif //IOC_TASK_OBJECT_READ_WRITE_RDMA_HPP
