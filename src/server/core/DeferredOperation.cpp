/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
//internal
#include "Object.hpp"
#include "DeferredOperation.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor of the deferred operation. Just make its initialization.
 * @param type Define the type of operation to be performed.
**/
DeferredOperation::DeferredOperation(DeferredOperationType type)
{
	this->type = type;
	this->buffer = NULL;
	this->size = 0;
	this->offset = 0;
	this->storageBackend = NULL;
	this->segment = NULL;
	this->object = NULL;
	this->ditryAction = DEFFERED_DIRTY_IGNORE;
}

/****************************************************/
/**
 * Attach the buffer informations.
 * @param buffer Define the buffer on which to apply the operation.
 * @param size Define the amount of data to move.
 * @param offset Define the offset of the operation in the storage backend.
**/
void DeferredOperation::setData(void * buffer, size_t size, size_t offset)
{
	//check
	assert(buffer != NULL);
	assert(size > 0);

	//set
	this->buffer = buffer;
	this->size = size;
	this->offset = offset;
}

/****************************************************/
/**
 * Attach the object information to the operation.
 * @param object Define on which object the operation will be done. This is more for debugging.
 * @param backend Define the storage backend to be used to make the operation.
 * @param segment Define the object segment on which to apply the dirty state.
**/
void DeferredOperation::setObjectInfos(const Object * object, StorageBackend * backend, ObjectSegment * segment)
{
	//check
	assert(object != NULL);
	assert(segment != NULL);

	//set
	this->object = object;
	this->storageBackend = backend;
	this->segment = segment;
}

/****************************************************/
/**
 * Define what to do with the dirty state of the object segment once the operation
 * has been performed.
**/
void DeferredOperation::setDitryAction(DeferredDirtyAction action)
{
	this->ditryAction = action;
}

/****************************************************/
/**
 * Apply the operation by making the requested call to pread()/pwrite().
**/
ssize_t DeferredOperation::run(void)
{
	//check
	assert(this->buffer != NULL);

	//nothing to do
	if (this->storageBackend == NULL)
		return this->size;

	//get object ID
	ObjectId objectId = this->object->getObjectId();

	//run it
	ssize_t ret = 0;
	switch(this->type) {
		case DEFEERRED_WRITE:
			if (this->storageBackend != NULL)
				ret = this->storageBackend->pwrite(objectId.high, objectId.low, this->buffer, this->size, this->offset);
			else
				ret = this->size;
			break;
		case DEFEERRED_READ:
			if (this->storageBackend != NULL)
				ret = this->storageBackend->pread(objectId.high, objectId.low, this->buffer, this->size, this->offset);
			else
				ret = this->size;
			break;
	}

	//on success
	if (ret == static_cast<ssize_t>(this->size)) {
		switch(this->ditryAction) {
			case DEFFERED_DIRTY_IGNORE: break;
			case DEFFERED_DIRTY_SET_TRUE: this->segment->setDirty(true); break;
			case DEFFERED_DIRTY_SET_FALSE: this->segment->setDirty(false); break;
		}
	}

	//return 
	return ret;
}

/****************************************************/
/**
 * Build a memory range information fromt the operation description to be
 * added to the memory ranges used for the task ordering and scheduling.
**/
MemRange DeferredOperation::buildMemRange(void)
{
	//check
	assert(this->buffer != NULL);
	assert(this->size > 0);

	//build
	return MemRange((size_t)this->buffer, this->size);
}

/****************************************************/
/**
 * Run all the oprations from the operation list.
**/
ssize_t DeferredOperationVector::runAll(void)
{
	ssize_t ret = 0;
	for (auto & op : *this)
		if (op.run() != static_cast<ssize_t>(op.getSize()))
			ret = -1;
	return ret;
}


/****************************************************/
/**
 * Build a list of memory ranges corresponding to all the contained
 * operations.
**/
MemRanges DeferredOperationVector::buildMemRanges(void)
{
	MemRanges ranges(this->size());
	for (auto & op : *this)
		ranges.push(op.buildMemRange());
	return ranges;
}
