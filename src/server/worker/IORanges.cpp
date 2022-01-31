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
IORange::IORange(void)
{
	this->address = 0;
	this->size = 0;
}

/****************************************************/
/**
 * @todo Make an optimization when count = 1 to have a local storage to avoid
 * the allocation.
**/
IORanges::IORanges(size_t count)
{
	//allocate
	if (count == 0)
		this->ranges = NULL;
	else
		this->ranges = new IORange[count];

	//init
	this->count = count;
	this->cursor = 0;
}

/****************************************************/
IORanges::IORanges(const IORange & uniqRange)
{
	//setup
	this->ranges = new IORange[1];
	this->ranges[0] = uniqRange;
	this->count = 1;
	this->cursor = 1;
}

/****************************************************/
IORanges::IORanges(IORanges && orig)
{
	//move
	this->ranges = orig.ranges;
	this->cursor = orig.cursor;
	this->count = orig.count;

	//reset orig
	orig.ranges = NULL;
	orig.count = 0;
	orig.cursor = 0;
}

/****************************************************/
IORanges::IORanges(const IORanges & orig)
{
	//setup
	this->ranges = new IORange[orig.count];
	this->count = orig.count;
	this->cursor = orig.cursor;

	//copy
	for (int i = 0 ; i < this->cursor ; i++)
		this->ranges[i] = orig.ranges[i];
}

/****************************************************/
IORanges::~IORanges(void)
{
	if (this->ranges != NULL)
		delete [] this->ranges;
}

/****************************************************/
IORanges & IORanges::operator=(IORanges && orig)
{
	//clear
	if (this->ranges != NULL)
		delete [] this->ranges;

	//move
	this->ranges = orig.ranges;
	this->cursor = orig.cursor;
	this->count = orig.count;

	//reset orig
	orig.ranges = NULL;
	orig.count = 0;
	orig.cursor = 0;

	//ret
	return *this;
}

/****************************************************/
IORanges & IORanges::operator=(IORanges & orig)
{
	//clear
	if (this->ranges != NULL)
		delete [] this->ranges;

	//setup
	this->ranges = new IORange[orig.count];
	this->count = orig.count;
	this->cursor = orig.cursor;

	//copy
	for (int i = 0 ; i < this->cursor ; i++)
		this->ranges[i] = orig.ranges[i];

	//ret
	return *this;
}

/****************************************************/
bool IORanges::ready(void) const
{
	return this->ranges != NULL && this->cursor == this->count && this->count > 0;
}

/****************************************************/
IORanges & IORanges::push(size_t address, size_t size)
{
	//check
	assert(address != 0);
	assert(size != 0);
	assert(this->cursor < this->count);

	//push
	this->ranges[this->cursor].address = address;
	this->ranges[this->cursor].size = size;

	//move
	this->cursor++;

	//ret for chaining
	return *this;
}

/****************************************************/
IORanges & IORanges::push(const IORange & range)
{
	//check
	assert(range.address != 0);
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
IORanges & IORanges::push(const IORanges & ranges)
{
	for (size_t i = 0 ; i < ranges.cursor ; i++)
		this->push(ranges.ranges[i]);
	return *this;
}

/****************************************************/
size_t IORanges::getCursor(void) const
{
	return this->cursor;
}

/****************************************************/
size_t IORanges::getCount(void) const
{
	return this->count;
}

/****************************************************/
IORange & IORanges::operator[](size_t index)
{
	assert(index < this->cursor);
	return this->ranges[index];
}
