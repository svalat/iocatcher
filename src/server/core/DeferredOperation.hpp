/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

#ifndef IOC_DEFERRED_OPERATION_HPP
#define IOC_DEFERRED_OPERATION_HPP

/****************************************************/
#include "StorageBackend.hpp"
#include "ObjectSegment.hpp"
#include "../worker/IORanges.hpp"
#include <vector>

/****************************************************/
namespace IOC
{

/****************************************************/
enum DeferredOperationType
{
	DEFEERRED_WRITE,
	DEFEERRED_READ,
};

/****************************************************/
enum DeferredDirtyAction
{
	DEFFERED_DIRTY_IGNORE,
	DEFFERED_DIRTY_SET_FALSE,
	DEFFERED_DIRTY_SET_TRUE,
};

/****************************************************/
class Object;

/****************************************************/
class DeferredOperation
{
	public:
		DeferredOperation(DeferredOperationType type);
		void setData(void * buffer, size_t size, size_t offset);
		void setObjectInfos(Object * object, StorageBackend * backend, ObjectSegment * segment);
		void setDitryAction(DeferredDirtyAction action);
		ssize_t run(void);
		size_t getSize(void) const {return this->size;};
		IORange buildMemRange(void);
	private:
		DeferredOperationType type;
		void * buffer;
		size_t size;
		size_t offset;
		StorageBackend * storageBackend;
		ObjectSegment * segment;
		Object * object;
		DeferredDirtyAction ditryAction;
};

/****************************************************/
class DeferredOperationList : public std::vector<DeferredOperation>
{
	public:
		ssize_t runAll(void);
		IORanges buildMemRanges(void);
};

}

#endif //IOC_DEFERRED_OPERATION_HPP
