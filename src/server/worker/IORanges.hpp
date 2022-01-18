/*****************************************************
			PROJECT  : IO Catcher
			VERSION  : 0.0.0-dev
			DATE     : 10/2020
			LICENSE  : ????????
*****************************************************/

#ifndef IOC_IO_RANGES_HPP
#define IOC_IO_RANGES_HPP

/****************************************************/
#include <cstdlib>
#include <deque>
#include "IORanges.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
struct IORange
{
	IORange(void);
	inline IORange(size_t address, size_t size);
	inline bool collide(const IORange & range) const;
	inline size_t end(void) const;
	size_t address;
	size_t size;
};

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
		IORanges & push(size_t address, size_t size);
		bool collide(const IORanges & ranges) const;
		bool ready(void) const;
	private:
		IORange * ranges;
		int count;
		int cursor;
};

/****************************************************/
inline IORange::IORange(size_t address, size_t size)
{
	this->address = address;
	this->size = size; 
};

/****************************************************/
inline bool IORange::collide(const IORange & range) const
{
	return ! (range.end() <= this->address || range.address >= this->end()); 
};

/****************************************************/
inline size_t IORange::end(void) const
{
	return this->address + this->size;
};

}

#endif //IOC_IO_RANGES_HPP
