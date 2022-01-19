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
void DeferredOperation::setObjectInfos(Object * object, StorageBackend * backend, ObjectSegment * segment)
{
	//check
	assert(object != NULL);
	assert(backend != NULL);
	assert(segment != NULL);

	//set
	this->object = object;
	this->storageBackend = backend;
	this->segment = segment;
}

/****************************************************/
void DeferredOperation::setDitryAction(DeferredDirtyAction action)
{
	this->ditryAction = action;
}

/****************************************************/
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
IORange DeferredOperation::buildIORange(void)
{
	//check
	assert(this->buffer != NULL);
	assert(this->size > 0);

	//build
	return IORange((size_t)this->buffer, this->size);
}


/****************************************************/
ssize_t DeferredOperationList::runAll(void)
{
	ssize_t ret = 0;
	for (auto & op : *this)
		if (op.run() != op.getSize())
			ret = -1;
	return ret;
}


/****************************************************/
IORanges DeferredOperationList::buildIORanges(void)
{
	IORanges ranges(this->size());
	for (auto & op : *this)
		ranges.push(op.buildIORange());
	return ranges;
}
