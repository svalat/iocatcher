/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include "../DeferredOperation.hpp"
#include "../Object.hpp"
#include "../../backends/MemoryBackendMalloc.hpp"
#include "../../backends/StorageBackendGMock.hpp"

/****************************************************/
using namespace IOC;
using namespace testing;

/****************************************************/
TEST(TestDeferredOperation, constructor)
{
	DeferredOperation op(DEFEERRED_WRITE);
}

/****************************************************/
TEST(TestDeferredOperation, run_write)
{
	MemoryBackendMalloc mback(NULL);
	ObjectId objectId(10, 20);
	StorageBackendGMock storage;
	Object object(&storage, &mback, objectId);

	//expect call to load
	EXPECT_CALL(storage, pread(10, 20, _, 500, 1000))
		.Times(1)
		.WillOnce(Return(500));

	//make request
	ObjectSegmentList lst;
	object.getBuffers(lst, 1000,500, ACCESS_READ);
	EXPECT_EQ(1, lst.size());
	ObjectSegmentDescr descr = lst.front();

	//build defered op
	DeferredOperation op(DEFEERRED_WRITE);
	op.setObjectInfos(&object, &storage, object.getObjectSegment(1000));
	op.setDitryAction(DEFFERED_DIRTY_SET_TRUE);
	op.setData(descr.ptr, descr.size, descr.offset);

	//expect call to write
	EXPECT_CALL(storage, pwrite(10, 20, _, 500, 1000))
		.Times(1)
		.WillOnce(Return(500));

	//run
	op.run();

	//check
	EXPECT_TRUE(object.getObjectSegment(1000)->isDirty());
}
