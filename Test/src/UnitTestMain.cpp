/*
 * UnitTestMain.cpp
 *
 *  Created on: Mar 31, 2015
 *      Author: qbus
 */

#include "gtest/gtest.h"
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <string>


GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
	google::ParseCommandLineFlags(&argc, &argv, true);

   	google::InitGoogleLogging("Test");

    int result = RUN_ALL_TESTS();

	google::ShutdownGoogleLogging();
	google::ShutDownCommandLineFlags();
	return result;
}


