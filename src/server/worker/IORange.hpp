/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_IO_RANGE_HPP
#define IOC_IO_RANGE_HPP

/****************************************************/
#include <cstdlib>
#include <deque>

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Define an IO range which is an adress with a size.
**/
struct IORange
{
	inline IORange(void);
	inline IORange(size_t offset, size_t size);
	inline bool collide(const IORange & range) const;
	inline IORange intersect(const IORange & range) const;
	inline size_t end(void) const;
	inline bool operator==(const IORange & other) const;
	/** Base offset or memory offset of the IO. **/
	size_t offset;
	/** Size of the IO. **/
	size_t size;
};

/****************************************************/
/** An alias to ease readability of the code. **/
typedef IORange MemRange;

/****************************************************/
/**
 * Default constructor.
**/
inline IORange::IORange(void)
{
	this->offset = 0;
	this->size = 0;
}

/****************************************************/
/**
 * Basic constructor.
 * @param offset The base offset or memory offset of the IO.
 * @param size The size of the IO.
**/
inline IORange::IORange(size_t offset, size_t size)
{
	//check out of bound
	assert(offset < offset + size);

	//set
	this->offset = offset;
	this->size = size; 
};

/****************************************************/
/**
 * Check if two IO ranges collide.
 * @param range Define the range to compare to the current one.
**/
inline bool IORange::collide(const IORange & range) const
{
	return ! (range.end() <= this->offset || range.offset >= this->end()); 
};

/****************************************************/
/**
 * Compute the end offset of the range.
**/
inline size_t IORange::end(void) const
{
	return this->offset + this->size;
};

/****************************************************/
/**
 * Compute the intersection of the given range with the current one.
 * @param range Define the range to intersect with.
**/
inline IORange IORange::intersect(const IORange & range) const
{
	assert(this->collide(range));
	size_t offset = std::max(this->offset, range.offset);
	size_t end = std::min(this->end(), range.end());
	return IORange(offset, end - offset);
}

/****************************************************/
/**
 * Define the equal operator to compare two ranges (for unit tests).
**/
bool IORange::operator==(const IORange & other) const
{
	return this->offset == other.offset && this->size == other.size;
}

}

#endif //IOC_IO_RANGE_HPP
