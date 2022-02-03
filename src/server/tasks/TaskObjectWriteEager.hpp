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
		/** Keep track of the connection to be able to send the response. **/
		LibfabricConnection * connection;
		/** Keep track of the request containing the data to write and to terminate it when the operation is finished. **/
		LibfabricClientRequest request;
		/** Keep track of container to know how to find the object. **/
		Container * container;
		/** Unpacked informations requested by the client. **/
		LibfabricObjReadWriteInfos objReadWrite;
		/** Track the status to know what to respond. **/
		bool status;
		/** List of segments to write on. **/
		ObjectSegmentList segments;
		/** To update the bandwidth stats. **/
		ServerStats * stats;
};

}

#endif //IOC_TASK_OBJECT_WRITE_EAGER_HPP
