/*****************************************************
*  PROJECT  : IO Catcher                             *
*  LICENSE  : Apache 2.0                             *
*  COPYRIGHT: 2020-2022 Bull SAS All rights reserved *
*****************************************************/

/****************************************************/
#include <gtest/gtest.h>
#include "../Config.hpp"

/****************************************************/
using namespace IOC;

/****************************************************/
TEST(TestConfig, options)
{
	//build object to test
	Config config;

	//setup test options
	const char * argv[] = {
		"ioc-server",
		"--nvdimm=/path1,/path2",
		"--no-consistency-check",
		"--active-polling",
		"--no-auth",
		"--verbose=core",
		"--merofile=./mero.rc",
		"--workers=8",
		"127.0.0.1"
	};

	//parse
	config.parseArgs(9, argv);

	//check status
	EXPECT_EQ(2, config.nvdimmMountPath.size());
	EXPECT_EQ("/path1", config.nvdimmMountPath[0]);
	EXPECT_EQ("/path2", config.nvdimmMountPath[1]);
	EXPECT_EQ("./mero.rc", config.meroRcFile);
	EXPECT_EQ(8, config.workers);
	EXPECT_FALSE(config.consistencyCheck);
	EXPECT_TRUE(config.activePolling);
	EXPECT_FALSE(config.clientAuth);
}

/****************************************************/
TEST(TestConfig, initForUnitTest)
{
	//build object to test
	Config config;
	config.initForUnitTests();
	EXPECT_TRUE(config.activePolling);
	EXPECT_EQ("127.0.0.1", config.listenIP);
	EXPECT_EQ(0, config.workers);
}
