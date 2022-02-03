/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

#ifndef IOC_LIBFABRIC_PRE_BUILT_RESPONSE_HPP
#define IOC_LIBFABRIC_PRE_BUILT_RESPONSE_HPP

/****************************************************/
#include "Protocol.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
// declare here due to cycle deps between LibfabricPreBuiltResponse & LibfabricConnection
class LibfabricConnection;

/****************************************************/
/**
 * To be used for the Read response because we want to serialize the response in the worker
 * thread to make the data copy at this call location and not calling Task::sendResponse()
 * making the copy in the network thread (which can be slower if the eager data content
 * is large).
 * This class allocate a buffer during the prepare phase, fill it and serialize during
 * the Task::runAction() phase and be send without more work in the Task::postAction() phase.
**/
class LibfabricPreBuiltResponse
{
	public:
		LibfabricPreBuiltResponse(LibfabricMessageType msgType, uint64_t lfClientId, LibfabricConnection * connection);
		~LibfabricPreBuiltResponse(void);
		void setStatus(int32_t status);
		void setData(const void * data, size_t size);
		void setBuffers(const LibfabricBuffer * buffers, size_t cntBuffers, size_t totalSize);
		void build(void);
		void send(void);
	private:
		/** Keep track of the connection to be used to send the response. **/
		LibfabricConnection * connection;
		/** Response structure to build the response before serialization. **/
		LibfabricResponse response;
		/** Type of message to be used to send the response. **/
		LibfabricMessageType msgType;
		/** Define the client to which we want to send the response. **/
		uint64_t lfClientId;
		/** Keep track of the buffer allocated from the domain to store the serialized message in. **/
		void * msgBuffer;
		/**
		 * Size of the serialized message to know how to call sendMessageRaw() when sending the response
		 * on the network.
		**/
		size_t msgSize;
};

}

#endif //IOC_LIBFABRIC_PRE_BUILT_RESPONSE_HPP
