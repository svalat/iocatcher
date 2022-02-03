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
/**
 * Define the type of operation to be performed.
**/
enum DeferredOperationType
{
	/** Decalare we will write the data to the storage backend. **/
	DEFEERRED_WRITE,
	/** Declare we will read the data from the storage backend. **/
	DEFEERRED_READ,
};

/****************************************************/
/**
 * Define what to apply after making the operation about the dirty
 * state of the realated object segment.
**/
enum DeferredDirtyAction
{
	/** Do nothing. **/
	DEFFERED_DIRTY_IGNORE,
	/** Set the objet segement as clean. **/
	DEFFERED_DIRTY_SET_FALSE,
	/** Set the objet segement as dirty. **/
	DEFFERED_DIRTY_SET_TRUE,
};

/****************************************************/
// pre-declare to break define order loop.
class Object;

/****************************************************/
/**
 * Define a deferred operation to be used in the tasking system. The purpose of this
 * class is to extract the buffer address information in the Task::runPrepare() phase of the task
 * in the networking thread an requiring access without locks to the object structures.
 * Then to perform the operation during the Task::runAction() which is called in the worker thread
 * without accessing the object structure which would have required locks.
**/
class DeferredOperation
{
	public:
		DeferredOperation(DeferredOperationType type);
		void setData(void * buffer, size_t size, size_t offset);
		void setObjectInfos(const Object * object, StorageBackend * backend, ObjectSegment * segment);
		void setDitryAction(DeferredDirtyAction action);
		ssize_t run(void);
		size_t getSize(void) const {return this->size;};
		IORange buildMemRange(void);
	private:
		/** Define the type of operation. **/
		DeferredOperationType type;
		/** Keep track of the buffer on which to apply the operation. **/
		void * buffer;
		/** Define the size of the amount of data to move. **/
		size_t size;
		/** Define the offset in the storage object (for pread/pwrite). **/
		size_t offset;
		/** Keep track of the storage backend to be used to make the operation. **/
		StorageBackend * storageBackend;
		/** 
		 * Keep track of the object segment on which we apply the operation in order
		 * to apply the dirty state.
		**/
		ObjectSegment * segment;
		/** Keep track of the object. **/
		const Object * object;
		/** Define what to do with the dirty state once the operation has been performed. **/
		DeferredDirtyAction ditryAction;
};

/****************************************************/
/**
 * Build a list of operations to be performed.
**/
class DeferredOperationVector : public std::vector<DeferredOperation>
{
	public:
		ssize_t runAll(void);
		MemRanges buildMemRanges(void);
};

}

#endif //IOC_DEFERRED_OPERATION_HPP
