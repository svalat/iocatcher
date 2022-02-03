/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

/****************************************************/
#include <cassert>
#include "IORanges.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
/**
 * Constructor givin the number of elements we expect to pre allocate
 * the storage.
 * If the size is 1 we use a local storage with no allocations.
**/
IORanges::IORanges(size_t count)
{
	//allocate
	if (count == 0)
		this->ranges = NULL;
	else if (count == 1)
		this->ranges = &this->uniqPreAllocated;
	else
		this->ranges = new IORange[count];

	//init
	this->count = count;
	this->cursor = 0;
}

/****************************************************/
/**
 * Constructor for a uniq range.
 * @param uniqRange The uniq range to put in the list.
**/
IORanges::IORanges(const IORange & uniqRange)
{
	//setup
	this->ranges = &this->uniqPreAllocated;
	this->ranges[0] = uniqRange;
	this->count = 1;
	this->cursor = 1;
}

/****************************************************/
/**
 * Move constructor.
 * @param orig The original ranges to move.
**/
IORanges::IORanges(IORanges && orig)
{
	//delete
	this->move(orig);
}

/****************************************************/
/**
 * Copy constructor.
 * @param orig The ranges to copy.
**/
IORanges::IORanges(const IORanges & orig)
{
	//delegate
	this->copy(orig);
}

/****************************************************/
/**
 * Destructor.
**/
IORanges::~IORanges(void)
{
	this->freemem();
}

/****************************************************/
/** Common implementation between move constructor and move assignement. **/
void IORanges::move(IORanges & orig)
{
	//move
	this->cursor = orig.cursor;
	this->count = orig.count;

	//case
	if (this->count == 1) {
		this->ranges = &this->uniqPreAllocated;
		this->ranges[0] = orig.ranges[0];
	} else {
		this->ranges = orig.ranges;
	}

	//reset orig
	orig.ranges = NULL;
	orig.count = 0;
	orig.cursor = 0;
}

/****************************************************/
/** Common implementation between copy constructor and assignement. **/
void IORanges::copy(const IORanges & orig)
{
	//setup
	this->count = orig.count;
	this->cursor = orig.cursor;

	//case
	if (this->count == 0) {
		//nothing
	} else if (this->count == 1) {
		this->ranges = &this->uniqPreAllocated;
	} else {
		this->ranges = new IORange[orig.count];
	}

	//copy
	for (int i = 0 ; i < this->cursor ; i++)
		this->ranges[i] = orig.ranges[i];
}

/****************************************************/
/** Helper function for destructor and assign operator. **/
void IORanges::freemem(void)
{
	if (this->ranges != NULL && this->ranges != &this->uniqPreAllocated) {
		delete [] this->ranges;
		this->ranges = NULL;
	}
}

/****************************************************/
/**
 * Asignement move operator.
 * @param orig The original ranges to move to the local one.
**/
IORanges & IORanges::operator=(IORanges && orig)
{
	//delegate
	this->freemem();
	this->move(orig);
	return *this;
}

/****************************************************/
/**
 * Standard assign operator.
 * @param orign The original ranges to copy.
**/
IORanges & IORanges::operator=(IORanges & orig)
{
	//delegate
	this->freemem();
	this->copy(orig);

	//ret
	return *this;
}

/****************************************************/
/**
 * Check if the ranges is ready, meaning we pushed as many elements than what we
 * decrived while allocating.
**/
bool IORanges::ready(void) const
{
	return this->ranges != NULL && this->cursor == this->count && this->count > 0;
}

/****************************************************/
/**
 * Push a range in the list.
 * @param offset The offset.
 * @param size The size.
**/
IORanges & IORanges::push(size_t offset, size_t size)
{
	//check
	assert(offset != 0);
	assert(size != 0);
	assert(this->cursor < this->count);

	//push
	this->ranges[this->cursor].offset = offset;
	this->ranges[this->cursor].size = size;

	//move
	this->cursor++;

	//ret for chaining
	return *this;
}

/****************************************************/
/**
 * Push a range to the list.
 * @param range The range to push.
**/
IORanges & IORanges::push(const IORange & range)
{
	//check
	assert(range.offset != 0);
	assert(range.size != 0);
	assert(this->cursor < this->count);

	//push
	this->ranges[this->cursor] = range;

	//move
	this->cursor++;

	//ret for chaining
	return *this;
}

/****************************************************/
/** 
 * @todo make an optimisation by sorting them
**/
bool IORanges::collide(const IORanges & ranges) const
{
	//check
	assert(this->cursor == this->count);
	assert(ranges.cursor == ranges.count);

	//search for collide
	for (int i = 0 ; i < this->count ; i++)
		for (int j = 0 ; j < ranges.count; j++)
			if (this->ranges[i].collide(ranges.ranges[j]))
				return true;

	//no collision
	return false;
}

/****************************************************/
/**
 * Merge the given ranges into the local one.
**/
IORanges & IORanges::push(const IORanges & ranges)
{
	//check
	assert(this->cursor + ranges.cursor <= this->count);
	
	//merge
	for (size_t i = 0 ; i < ranges.cursor ; i++)
		this->push(ranges.ranges[i]);

	//ok
	return *this;
}

/****************************************************/
/**
 * Return the cursor position.
**/
size_t IORanges::getCursor(void) const
{
	return this->cursor;
}

/****************************************************/
/**
 * Return the number of element for which we made the allocation.
**/
size_t IORanges::getCount(void) const
{
	return this->count;
}

/****************************************************/
/**
 * Implement the braces operator.
 * @param index Index of the desierd element.
**/
IORange & IORanges::operator[](size_t index)
{
	assert(index < this->cursor);
	return this->ranges[index];
}
