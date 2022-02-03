/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_IO_RANGES_HPP
#define IOC_IO_RANGES_HPP

/****************************************************/
#include <cstdlib>
#include <deque>
#include "IORange.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
/**
 * Build a vector of ranges.
**/
class IORanges
{
	public:
		IORanges(IORanges && orig);
		IORanges(const IORanges & orig);
		IORanges(size_t count);
		IORanges(const IORange & uniqRange);
		~IORanges(void);
		IORanges & push(const IORange & range);
		IORanges & push(size_t offset, size_t size);
		IORanges & push(const IORanges & ranges);
		bool collide(const IORanges & ranges) const;
		bool ready(void) const;
		IORanges & operator=(IORanges && orig);
		IORanges & operator=(IORanges & orig);
		size_t getCursor(void) const;
		size_t getCount(void) const;
		IORange & operator[](size_t index);
	private:
		void move(IORanges & orig);
		void copy(const IORanges & orig);
		void freemem(void);
	private:
		/** Pointer to the allocated ranges array. **/
		IORange * ranges;
		/** Provide a local storage if we use a uniq range to avoid a malloc(). **/
		IORange uniqPreAllocated;
		/** Number of elements in the rane array. **/
		int count;
		/** Position of the cursor for the push operations. **/
		int cursor;
};

/****************************************************/
/**
 * Simple typedef to be more explicit on what we manipulate.
**/
typedef IORanges MemRanges;

}

#endif //IOC_IO_RANGES_HPP
