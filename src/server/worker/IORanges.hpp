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
		IORange * ranges;
		int count;
		int cursor;
};

/****************************************************/
typedef IORanges MemRanges;

}

#endif //IOC_IO_RANGES_HPP
