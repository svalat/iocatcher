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
/**
 * Define an object range which join an object ID with an access range.
**/
struct ObjectRange
{
	inline ObjectRange(void);
	inline ObjectRange(const ObjectId & objectId, size_t address, size_t size);
	inline bool collide(const ObjectRange & range) const;
	/** Define the object ID it applies to. **/
	ObjectId objectId;
	/** Define the access range to protect. **/
	IORange range;
};

/****************************************************/
/**
 * Define a list of object ranges to handle in one go.
 * This implemented is optimized to avoid a malloc for 1 or 2 ranges which
 * cover all the cases for the current IOC protocol.
**/
class ObjectRanges
{
	public:
		ObjectRanges(ObjectRanges && orig);
		ObjectRanges(const ObjectRanges & orig);
		ObjectRanges(size_t count);
		ObjectRanges(const ObjectRange & uniqRange);
		~ObjectRanges(void);
		ObjectRanges & push(const ObjectRange & objectRange);
		ObjectRanges & push(const ObjectId & objectId, size_t offset, size_t size);
		bool collide(const ObjectRanges & objectRanges) const;
		bool ready(void) const;
	private:
		/** Pointer to the range array. **/
		ObjectRange * objectRanges;
		/** Local storage to avoid a dynamic allocation if we have 1 or 2 elements. **/
		ObjectRange biRangesCase[2];
		/** Number of element we allocated. **/
		int count;
		/** Cursor to be used by the push() operations. **/
		int cursor;
};

/****************************************************/
/** Default constructor. **/
ObjectRange::ObjectRange(void)
{

}

/****************************************************/
/**
 * Construct an object range.
 * @param objectId Define the object ID to protect.
 * @param offset Define the offset of the range to protect.
 * @param size Define the size of the range to protect.
**/
inline ObjectRange::ObjectRange(const ObjectId & objectId, size_t offset, size_t size)
                   :objectId(objectId)
                   ,range(offset, size)
{
};

/****************************************************/
/**
 * Check of we collide with the same object and same range.
**/
inline bool ObjectRange::collide(const ObjectRange & objectRange) const
{
	return this->objectId == objectRange.objectId && this->range.collide(objectRange.range);
};

}

#endif //IOC_OBJECT_RANGES_HPP
