/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
//local
#include "LibfabricPreBuiltResponse.hpp"
#include "LibfabricConnection.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Constructor of a pre-built response.
 * @param msgType The type of message to be used when sending the response.
 * @param lfClientId The client ID to which we want to send the message.
 * @param connection The connection to be used to allocate the buffer and send the message.
**/
LibfabricPreBuiltResponse::LibfabricPreBuiltResponse(LibfabricMessageType msgType, uint64_t lfClientId, LibfabricConnection * connection)
{
	this->connection = connection;
	this->msgType = msgType;
	this->lfClientId = lfClientId;
	this->response.initStatusOnly(0);
	this->msgBuffer = NULL;
	this->msgSize = 0;
}

/****************************************************/
/**
 * Destructor, it delete the potential fragments if there is some.
**/
LibfabricPreBuiltResponse::~LibfabricPreBuiltResponse(void)
{
	if (this->response.optionalDataFragments != NULL)
		delete [] this->response.optionalDataFragments;
}

/****************************************************/
/**
 * Define the status of the response.
 * To be called before calling build() which serialize the response.
**/
void LibfabricPreBuiltResponse::setStatus(int32_t status)
{
	this->response.status = status;
}

/****************************************************/
/**
 * Attach a data buffer to be pasted in the response message.
 * To be called before calling build() which serialize the response.
 * Remark the buffer will not be deleted by this class.
 * @param data Pointer to the buffer.
 * @param size Size of the data.
**/
void LibfabricPreBuiltResponse::setData(const void * data, size_t size)
{
	//check
	assert(data != NULL);
	assert(size != 0);

	//setup
	this->response.msgHasData = true;
	this->response.optionalData = static_cast<const char*>(data);
	this->response.msgDataSize = size;
}

/****************************************************/
/**
 * Attach a list of fragments (buffers) to be pasted in the response message.
 * To be called before calling build() which serialize the response.
 * Remark the buffer will be deleted by this class.
 * @param buffers Array of buffer descriptors to aggregate.
 * @param cntBuffers Number of buffers in the array.
**/
void LibfabricPreBuiltResponse::setBuffers(const LibfabricBuffer * buffers, size_t cntBuffers, size_t totalSize)
{
	//check
	assert(buffers != NULL);
	assert(cntBuffers != 0);

	//setup
	this->response.msgHasData = true;
	this->response.msgDataSize = totalSize;
	this->response.optionalDataFragments = buffers;
	this->response.optionalDataFragmentCount = cntBuffers;
}

/****************************************************/
/**
 * To be called to effectively serialize the response in the send buffer.
**/
void LibfabricPreBuiltResponse::build(void)
{
	//get a buffer
	LibfabricDomain & domain = this->connection->getDomain();
	void * buffer = domain.getMsgBuffer();
	size_t bufferSize = domain.getMsgBufferSize();

	//build the header
	LibfabricMessageHeader header;
	this->connection->fillProtocolHeader(header, msgType);

	//serialize
	Serializer serializer(buffer, bufferSize);
	serializer.apply("header", header);
	serializer.apply("data", this->response);

	//extract size
	size_t finalSize = serializer.getCursor();

	//store
	this->msgBuffer = buffer;
	this->msgSize = finalSize;
}

/****************************************************/
/**
 * Once we have called build() we can now send the message on the wire.
**/
void LibfabricPreBuiltResponse::send(void)
{
	//check
	assert(this->msgSize != 0);

	//build post action
	LibfabricPostActionNop * postAction = new LibfabricPostActionNop(LF_WAIT_LOOP_UNBLOCK);

	//register it to be freed when the postaction will be deleted
	postAction->attachDomainBuffer(this->connection, this->msgBuffer);

	//send the message
	this->connection->sendRawMessage(this->msgBuffer, this->msgSize, this->lfClientId, postAction);
}

}
