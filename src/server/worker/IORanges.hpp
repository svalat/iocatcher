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

/****************************************************/
namespace IOC
{

/****************************************************/
struct IORange
{
	IORange(void);
	inline IORange(size_t address, size_t size);
	inline bool collide(const IORange & range) const;
	inline static IORange intersect(const IORange & range1, const IORange & range2);
	inline size_t end(void) const;
	inline bool operator==(const IORange & other) const;
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

/****************************************************/
IORange IORange::intersect(const IORange & range1, const IORange & range2)
{
	assert(range1.collide(range2));
	size_t offset = std::max(range1.address, range2.address);
	size_t end = std::min(range1.end(), range2.end());
	return IORange(offset, end - offset);
}

/****************************************************/
bool IORange::operator==(const IORange & other) const
{
	return this->address == other.address && this->size == other.size;
}

}

#endif //IOC_IO_RANGES_HPP
