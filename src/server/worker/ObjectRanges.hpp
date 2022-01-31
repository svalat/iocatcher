/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2022 Sebastien Valat                   *
*****************************************************/

#ifndef IOC_OBJECT_RANGES_HPP
#define IOC_OBJECT_RANGES_HPP

/****************************************************/
#include <cstdlib>
#include <deque>
#include "../core/Object.hpp"
#include "IORanges.hpp"

/****************************************************/
namespace IOC
{

/****************************************************/
struct ObjectRange
{
	ObjectRange(void);
	inline ObjectRange(const ObjectId & objectId, size_t address, size_t size);
	inline bool collide(const ObjectRange & range) const;
	ObjectId objectId;
	IORange range;
};

/****************************************************/
class ObjectRanges
{
	public:
		ObjectRanges(ObjectRanges && orig);
		ObjectRanges(const ObjectRanges & orig);
		ObjectRanges(size_t count);
		ObjectRanges(const ObjectRange & uniqRange);
		~ObjectRanges(void);
		ObjectRanges & push(const ObjectRange & objectRange);
		ObjectRanges & push(const ObjectId & objectId, size_t address, size_t size);
		bool collide(const ObjectRanges & objectRanges) const;
		bool ready(void) const;
	private:
		ObjectRange * objectRanges;
		int count;
		int cursor;
};

/****************************************************/
inline ObjectRange::ObjectRange(const ObjectId & objectId, size_t address, size_t size)
                   :objectId(objectId)
                   ,range(address, size)
{
};

/****************************************************/
inline bool ObjectRange::collide(const ObjectRange & objectRange) const
{
	return this->objectId == objectRange.objectId && this->range.collide(objectRange.range);
};

}

#endif //IOC_OBJECT_RANGES_HPP
