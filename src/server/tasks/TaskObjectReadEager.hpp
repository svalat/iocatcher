/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_TASK_OBJECT_READ_EAGER_HPP
#define IOC_TASK_OBJECT_READ_EAGER_HPP

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
 * Taskify the object read eager operation.
**/
class TaskObjectReadEager : public TaskDeferredOps
{
	public:
		TaskObjectReadEager(LibfabricConnection * connection, uint64_t lfClientId, Container * container, ServerStats * stats, LibfabricObjReadWriteInfos objReadWrite);
	protected:
		virtual void runPostAction(void) override;
		virtual void runAction(void) override;
		virtual void runPrepare(void) override;
	private:
		void performMemcpyOps(void);
	private:
		/** Keep track of the container to be used to find the object. **/
		Container * container;
		/** Request informations. **/
		LibfabricObjReadWriteInfos objReadWrite;
		/** Track the status to know what to respond. **/
		bool status;
		ObjectSegmentList segments;
		ServerStats * stats;
		LibfabricPreBuiltResponse preBuiltResponse;
};

}

#endif //IOC_TASK_OBJECT_READ_EAGER_HPP
