/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <cassert>
#include "ObjectRanges.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
ObjectRange::ObjectRange(void)
{
}

/****************************************************/
/**
 * @todo Make an optimization when count = 1 to have a local storage to avoid
 * the allocation.
**/
ObjectRanges::ObjectRanges(size_t count)
{
	//allocate
	if (count == 0)
		this->objectRanges = NULL;
	else
		this->objectRanges = new ObjectRange[count];

	//init
	this->count = count;
	this->cursor = 0;
}

/****************************************************/
ObjectRanges::ObjectRanges(const ObjectRange & uniqRange)
{
	//setup
	this->objectRanges = new ObjectRange[1];
	this->objectRanges[0] = uniqRange;
	this->count = 1;
	this->cursor = 1;
}

/****************************************************/
ObjectRanges::ObjectRanges(ObjectRanges && orig)
{
	//move
	this->objectRanges = orig.objectRanges;
	this->cursor = orig.cursor;
	this->count = orig.count;

	//reset orig
	orig.objectRanges = NULL;
	orig.count = 0;
	orig.cursor = 0;
}

/****************************************************/
ObjectRanges::ObjectRanges(const ObjectRanges & orig)
{
	//setup
	this->objectRanges = new ObjectRange[orig.count];
	this->count = orig.count;
	this->cursor = orig.cursor;

	//copy
	for (int i = 0 ; i < this->cursor ; i++)
		this->objectRanges[i] = orig.objectRanges[i];
}

/****************************************************/
ObjectRanges::~ObjectRanges(void)
{
	if (this->objectRanges != NULL)
		delete [] this->objectRanges;
}

/****************************************************/
bool ObjectRanges::ready(void) const
{
	return this->objectRanges != NULL && this->cursor == this->count && this->count > 0;
}

/****************************************************/
ObjectRanges & ObjectRanges::push(const ObjectId & objectId, size_t address, size_t size)
{
	//check
	assert(address != 0);
	assert(size != 0);
	assert(this->cursor < this->count);

	//push
	this->objectRanges[this->cursor].objectId = objectId;
	this->objectRanges[this->cursor].range.address = address;
	this->objectRanges[this->cursor].range.size = size;

	//move
	this->cursor++;

	//ret for chaining
	return *this;
}

/****************************************************/
ObjectRanges & ObjectRanges::push(const ObjectRange & objectRange)
{
	//check
	assert(this->cursor < this->count);

	//push
	this->objectRanges[this->cursor] = objectRange;

	//move
	this->cursor++;

	//ret for chaining
	return *this;
}

/****************************************************/
/** 
 * @todo make an optimisation by sorting them
**/
bool ObjectRanges::collide(const ObjectRanges & ranges) const
{
	//check
	assert(this->cursor == this->count);
	assert(ranges.cursor == ranges.count);

	//search for collide
	for (int i = 0 ; i < this->count ; i++)
		for (int j = 0 ; j < ranges.count; j++)
			if (this->objectRanges[i].collide(ranges.objectRanges[j]))
				return true;

	//no collision
	return false;
}
