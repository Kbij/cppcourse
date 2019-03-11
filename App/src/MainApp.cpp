/*
 * MainApp.cpp
 *
 */

#include <glog/logging.h>
#include <gflags/gflags.h>
#include <string>

int main (int argc, char* argv[])
{
   	google::InitGoogleLogging("App");
	std::string usage("App. Sample usage:\n");
	usage += argv[0];

	google::SetUsageMessage(usage);
	google::SetVersionString("1.0");
	google::ParseCommandLineFlags(&argc, &argv, true);


	LOG(INFO) << "Bye...";
	google::ShutdownGoogleLogging();
	google::ShutDownCommandLineFlags();

	return EXIT_SUCCESS;
}

