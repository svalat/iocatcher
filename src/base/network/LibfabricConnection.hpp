/*****************************************************
			 PROJECT  : IO Catcher
			 LICENSE  : Apache 2.0
			COPYRIGHT: 2020 Bull SAS
			 COPYRIGHT: 2020 Bull SAS
*****************************************************/

#ifndef IOC_LIBFABRIC_CONNECTION_HPP
#define IOC_LIBFABRIC_CONNECTION_HPP

/****************************************************/
//std
#include <functional>
#include <map>
//libfabric
#include <rdma/fabric.h>
#include <rdma/fi_errno.h>
#include <rdma/fi_endpoint.h>
//local
#include "LibfabricDomain.hpp"
#include "Protocol.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
#define IOC_LF_SERVER_ID 0

/****************************************************/
class LibfabricConnection;

/****************************************************/
enum LibfabricActionResult
{
	LF_WAIT_LOOP_KEEP_WAITING = 0,
	LF_WAIT_LOOP_UNBLOCK = 1,
};

/****************************************************/
class LibfabricPostAction
{
	public:
		LibfabricPostAction(void) {this->bufferId = -1; this->connection = NULL;};
		virtual ~LibfabricPostAction(void) {};
		virtual LibfabricActionResult runPostAction(void) = 0;
		void registerBuffer(LibfabricConnection * connection, bool isRecv, size_t bufferId);
		void freeBuffer(void);
	protected:
		LibfabricConnection * connection;
		bool isRecv;
		size_t bufferId;
};

/****************************************************/
class LibfabricPostActionFunction : public LibfabricPostAction
{
	public:
		LibfabricPostActionFunction(std::function<LibfabricActionResult(void)> function) {this->function = function;};
		virtual LibfabricActionResult runPostAction(void);
	private:
		std::function<LibfabricActionResult(void)> function;
};

/****************************************************/
class LibfabricConnection
{
	public:
		LibfabricConnection(LibfabricDomain * lfDomain, bool wait);
		~LibfabricConnection(void);
		void postRecives(size_t size, int count);
		void joinServer(void);
		void poll(bool waitMsg);
		void setHooks(std::function<void(int)> hookOnEndpointConnect);
		void sendMessage(void * buffer, size_t size, int destinationEpId, std::function<LibfabricActionResult(void)> postAction);
		void rdmaRead(int destinationEpId, void * localAddr, void * remoteAddr, uint64_t remoteKey, size_t size, std::function<LibfabricActionResult(void)> postAction);
		void rdmaReadv(int destinationEpId, struct iovec * iov, int count, void * remoteAddr, uint64_t remoteKey, std::function<LibfabricActionResult(void)> postAction);
		void rdmaWrite(int destinationEpId, void * localAddr, void * remoteAddr, uint64_t remoteKey, size_t size, std::function<LibfabricActionResult(void)> postAction);
		void rdmaWritev(int destinationEpId, struct iovec * iov, int count, void * remoteAddr, uint64_t remoteKey, std::function<LibfabricActionResult(void)> postAction);
		void repostRecive(size_t id);
		void registerHook(int messageType, std::function<LibfabricActionResult(int, size_t, void*)> function);
		void unregisterHook(int messageType);
		int getClientId(void) { return clientId;};
		LibfabricDomain & getDomain(void) {return *lfDomain;};
		void setUsed(bool used) {this->used = used;};
		bool getUsed(void) {return this->used;}
		void setTcpClientInfos(uint64_t tcpClientId, uint64_t tcpClientKey);
		void fillProtocolHeader(LibfabricMessageHeader & header, int type);
		ClientRegistry & getClientRegistry(void);
		void setCheckClientAuth(bool value);
		void setOnBadAuth(std::function<LibfabricActionResult(void)> hookOnBadAuth);
	private:
		bool pollRx(void);
		bool pollTx(void);
		int pollForCompletion(struct fid_cq * cq, struct fi_cq_msg_entry* entry, bool wait);
		bool onRecv(size_t id);
		void onSent(void * buffer);
		void onConnInit(LibfabricMessage * message);
		bool checkAuth(LibfabricMessage * message, int clientId, int id);
	private:
		LibfabricDomain * lfDomain;
		/** Completion queue. **/
		fid_cq *cq;
		/** Address vector. **/
		fid_av *av;
		/** Endpoint **/
		fid_ep *ep;
		/** If use wait **/
		bool wait;
		/** buffers **/
		char ** recvBuffers;
		size_t recvBuffersCount;
		size_t recvBuffersSize;
		std::map<int, fi_addr_t> remoteLiAddr;
		int nextEndpointId;
		std::function<void(int)> hookOnEndpointConnect;
		std::function<LibfabricActionResult(void)> hookOnBadAuth;
		int clientId;
		std::map<size_t, std::function<bool(int, size_t, void*)>> hooks;
		bool used;
		uint64_t tcpClientId;
		uint64_t tcpClientKey;
		ClientRegistry clientRegistry;
		bool checkClientAuth;
};

}

#endif //IOC_LIBFABRIC_CONNECTION_HPP
