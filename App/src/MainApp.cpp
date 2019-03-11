/*
 * MainApp.cpp
 *
 *  Created on: Nov 23, 2015
 *      Author: qbus
 */

#include "AppVersion.h"
#include "WatchDog/WatchDogFactory.h"
#include "WatchDog/WatchDogSlaveIf.h"
#include "InterComm/InterCommFactory.h"
#include "InterComm/InterCommAsyncIf.h"
#include "Managers/StatusFactory.h"
#include "Managers/ConfigManagerFactory.h"
#include "Managers/ManagerFactory.h"
#include "Managers/StatusSenderIf.h"
#include "Managers/AlarmSourceIf.h"
#include "Qbus/QbusFactory.h"
#include "Qbus/QbusSystemIf.h"
#include "Common/Utilities.h"
#include "Constants.h"
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>


DEFINE_bool(daemon, false, "Run Qbus as Daemon");
DEFINE_string(pidfile,"","Pid file when running as Daemon");
DEFINE_string(storagedir, ".", "Storage directory");

const std::string APP_QBUS = "Qbus";

void signal_handler(int sig);
void daemonize();

std::condition_variable exitCv;
std::mutex exitMutex;
bool exitMain = false;
int pidFilehandle;

const std::string RUNTIME_SETTINGS = "logging.json";

void readRuntimeLogginSettings()
{
    std::stringstream fileName;
    if (FLAGS_storagedir.size() > 0)
    {
    	fileName << FLAGS_storagedir <<  "/" << RUNTIME_SETTINGS;
    }
    else
    {
    	fileName << RUNTIME_SETTINGS;
    }
    CommonNs::readRuntimeLoggingSettings(fileName.str(), APP_QBUS);
}

void signal_handler(int sig)
{
	switch(sig)
    {
		case SIGHUP:
			readRuntimeLogginSettings();
			break;
		case SIGINT:
		case SIGTERM:
		case SIGKILL:
			LOG(INFO) << "Application exiting";
			{
				std::lock_guard<std::mutex> lk(exitMutex);
				exitMain = true;
				exitCv.notify_one();
			}
			break;
		default:
			LOG(WARNING) << "Unhandled signal " << strsignal(sig);
			break;
    }
}

void registerSignals()
{
    struct sigaction newSigAction;
    sigset_t newSigSet;

    /* Set signal mask - signals we want to block */
    sigemptyset(&newSigSet);
    sigaddset(&newSigSet, SIGCHLD);  /* ignore child - i.e. we don't need to wait for it */
    sigaddset(&newSigSet, SIGTSTP);  /* ignore Tty stop signals */
    sigaddset(&newSigSet, SIGTTOU);  /* ignore Tty background writes */
    sigaddset(&newSigSet, SIGTTIN);  /* ignore Tty background reads */
    sigaddset(&newSigSet, SIGKILL);  /* Netbeans terminate process */
    sigprocmask(SIG_BLOCK, &newSigSet, NULL);   /* Block the above specified signals */

    /* Set up a signal handler */
    newSigAction.sa_handler = signal_handler;
    sigemptyset(&newSigAction.sa_mask);
    newSigAction.sa_flags = 0;

    /* Signals to handle */
    sigaction(SIGHUP, &newSigAction, NULL);     /* catch hangup signal */
    sigaction(SIGTERM, &newSigAction, NULL);    /* catch term signal */
    sigaction(SIGINT, &newSigAction, NULL);     /* catch interrupt signal */
}

void daemonize()
{
    int pid, sid, i;
    char str[10];
    /* Check if parent process id is set */
    if (getppid() == 1)
    {
        /* PPID exists, therefore we are already a daemon */
        return;
    }

    /* Fork*/
    pid = fork();

    if (pid < 0)
    {
        /* Could not fork */
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
    {
        /* Child created ok, so exit parent process */
        exit(EXIT_SUCCESS);
    }

    /* Get a new process group */
    sid = setsid();

    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    /* close all descriptors */
    for (i = getdtablesize(); i >= 0; --i)
    {
        close(i);
    }

    /* Route I/O connections */
    /* Open STDIN */
    i = open("/dev/null", O_RDWR);
    /* STDOUT */
    CommonNs::ignore_result(dup(i));
    /* STDERR */
    CommonNs::ignore_result(dup(i));

//    chdir(rundir); /* change running directory */

    /* Ensure only one copy */
    pidFilehandle = open(FLAGS_pidfile.c_str(), O_RDWR|O_CREAT, 0600);

    if (pidFilehandle == -1 )
    {
        /* Couldn't open lock file */
    	LOG(ERROR) << "Could not open PID lock file " << FLAGS_pidfile << ", exiting";
        exit(EXIT_FAILURE);
    }

    /* Try to lock file */
    if (lockf(pidFilehandle,F_TLOCK,0) == -1)
    {
        /* Couldn't get lock on lock file */
    	LOG(WARNING) << "Could not lock PID lock file " << FLAGS_pidfile << ", exiting";
        exit(EXIT_FAILURE);
    }

    /* Get and format PID */
    sprintf(str,"%d\n",getpid());

    /* write pid to lockfile */
    CommonNs::ignore_result(write(pidFilehandle, str, strlen(str)));
}

int main (int argc, char* argv[])
{
	std::string usage("Qbus HCB Qbus. Sample usage:\n");
	usage += argv[0];

	google::SetUsageMessage(usage);
	google::SetVersionString(QBUS_VERSION);
	google::ParseCommandLineFlags(&argc, &argv, true);
	if (FLAGS_daemon)
	{
		daemonize();
	}
	registerSignals();

    try
    {
		google::InitGoogleLogging("Qbus");
	//	google::InstallFailureSignalHandler();

		const std::string APP_INFO("Ubie Qbus App (" + QBUS_VERSION + ", " + __DATE__ + ", " + __TIME__ + ")");
		LOG(INFO) << APP_INFO;
		LOG(INFO) << std::string(APP_INFO.size(), '=');

    	InterCommNs::InterCommAsyncIf* interComm = InterCommNs::InterCommFactory::createInterCommAsync(APP_QBUS);
    	interComm->createMailbox(APP_QBUS, MAILBOX_SIZE_BYTES);
    	interComm->registerMailbox(APP_MAIN);
    	interComm->registerMailbox(APP_LOGIC);

    	InterCommNs::InterCommAsyncIf* wdInterComm = InterCommNs::InterCommFactory::createInterCommAsync(APP_QBUS + "WD");
    	wdInterComm->createMailbox(APP_QBUS + "WD", WD_MAILBOX_SIZE_BYTES);
    	wdInterComm->registerMailbox(APP_WATCHDOGMASTER);

    	WatchDogNs::WatchDogSlaveIf* watchDog = WatchDogNs::WatchDogFactory::createSlaveWatchdog(wdInterComm, APP_WATCHDOGMASTER, APP_QBUS);
    	ManagersNs::StatusSenderIf* statusSender = ManagersNs::StatusFactory::createStatusSender(interComm, {APP_MAIN, APP_LOGIC}, watchDog);
    	ManagersNs::ConfigClientIf* configClient = ManagersNs::ConfigManagerFactory::createConfigClient(interComm,APP_MAIN, watchDog);
    	std::string qbusConfig = "qbushw.json";
    	std::string translationFile = "messages.json";
    	if (FLAGS_storagedir.size() > 0)
    	{
    		qbusConfig = FLAGS_storagedir + "/qbushw.json";
    		translationFile = FLAGS_storagedir + "/messages.json";
    	}
    	ManagersNs::AlarmSourceIf* alarmSource =  ManagersNs::ManagerFactory::createAlarmSource(interComm, APP_MAIN, translationFile);
    	QbusNs::QbusSystemIf* qbusSystem = QbusNs::QbusFactory::createQbusSystem(qbusConfig,"QFB",statusSender,configClient, watchDog, FLAGS_storagedir, alarmSource);
    	//Notify that we are successfully started
    	watchDog->applicationStarted();

    	// Wait until application stopped by a signal handler
        std::unique_lock<std::mutex> lk(exitMutex);
        exitCv.wait(lk, []{return exitMain;});


        //destroy Qbus
        QbusNs::QbusFactory::destroyQbusSystem(qbusSystem);
        ManagersNs::ManagerFactory::destroyAlarmSource(alarmSource);
        ManagersNs::ConfigManagerFactory::destroyConfigClient(configClient);
        ManagersNs::StatusFactory::destroyStatusSender(statusSender);
        WatchDogNs::WatchDogFactory::destroySlaveWatchdog(watchDog);
        InterCommNs::InterCommFactory::destroyInterCommAsync(wdInterComm);
    	InterCommNs::InterCommFactory::destroyInterCommAsync(interComm);
    }
	catch (std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << std::endl;
		LOG(ERROR) << "Exception: " << ex.what();
	}

	LOG(INFO) << "Bye...";
	google::ShutdownGoogleLogging();
	google::ShutDownCommandLineFlags();

	return EXIT_SUCCESS;
}

