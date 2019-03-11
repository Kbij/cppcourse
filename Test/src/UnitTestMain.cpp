/*
 * UnitTestMain.cpp
 *
 *  Created on: Mar 31, 2015
 *      Author: qbus
 */

#include "InterComm/IntMessage.h"
#include "InterComm/StringMessage.h"
#include "InterComm/InterCommFactory.h"
#include "InterComm/InterCommIf.h"
#include "InterComm/MessageReceiverIf.h"
#include "CrashTest.h"
#include "gtest/gtest.h"
#include "MessageReceiverStub.h"
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <iostream>
#include  <thread>
#include <algorithm>
#include <mutex>

std::string mainApplication;
DEFINE_bool(quickTests, false, "Perform only the quick tests");
DEFINE_bool(fullTests, false, "Perform only the quick tests");
DEFINE_bool(slave1, false, "Run as Slave1");
DEFINE_bool(slave2, false, "Run as Slave2");
DEFINE_bool(crashwhilereceiving, false, "Crash after x receive operations");
DEFINE_bool(crashwhilesending, false, "Crash after x sending operations");
DEFINE_int32(messagecount, 0, "Crash after x messages");

int unitTestSlave1()
{
	google::InitGoogleLogging("InterCommSlave1");

	InterCommNs::InterCommIf* interComm = InterCommNs::InterCommFactory::createInterComm("Slave");
	interComm->registerMailbox("Master");
	interComm->registerMailbox("Slave");
	int messageCount = 0;
	int receiveCount = 0;
	int waitTimeMs = 0;

	// Wait for the master to send its messagecount
	while (messageCount == 0 && waitTimeMs < 5000)
	{
		if (interComm->hasMessage())
		{
			InterCommNs::MessageIf* objectOut = interComm->receive();
		    if(InterCommNs::IntMessage* intMessage = dynamic_cast<InterCommNs::IntMessage*>(objectOut))
		    {
		    	messageCount = intMessage->intValue();
		    }
		    delete objectOut;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			waitTimeMs += 10;
		}
	}

	waitTimeMs = 0;
	if (messageCount > 0)
	{
		while (receiveCount < messageCount && waitTimeMs < 10000)
		{
			if (interComm->hasMessage())
			{
				waitTimeMs = 0;
				InterCommNs::MessageIf* objectOut = interComm->receive();
				++receiveCount;
				delete objectOut;
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::microseconds(5));
				++waitTimeMs;
			}
		}
	}

	interComm->send("Master", new InterCommNs::IntMessage(receiveCount));
	// Wait some time, to allow the slave to send to the master
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	InterCommNs::InterCommFactory::destroyInterComm(interComm);

//	std::cout << "Slave1 done" << std::endl;
	return 0;
}
int unitTestSlave2()
{
	google::InitGoogleLogging("InterCommSlave1");

	InterCommNs::InterCommIf* interComm = InterCommNs::InterCommFactory::createInterComm("Slave");
	interComm->registerMailbox("Master");
	interComm->registerMailbox("Slave");
	InterCommNs::MessageReceiverStub receiver;


	int messageCount = 0;
	int waitTimeMs = 0;

	// Wait for the master to send its messagecount
	while (messageCount == 0 && waitTimeMs < 5000)
	{
		if (interComm->hasMessage())
		{
			InterCommNs::MessageIf* objectOut = interComm->receive();
		    if(InterCommNs::IntMessage* intMessage = dynamic_cast<InterCommNs::IntMessage*>(objectOut))
		    {
		    	messageCount = intMessage->intValue();
		    }
		    delete objectOut;
            LOG(INFO)<< "Slave messagecount received: " << messageCount;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			waitTimeMs += 10;
		}
	}

	interComm->registerNotificationMessageType(&receiver, "");
	//receiver.reset();
	waitTimeMs = 0;
	if (messageCount > 0)
	{
		// messages.messagesReceived() gets updated by the receive thread of the intercomm
		while (receiver.messagesReceived() < messageCount && waitTimeMs < 30000)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			waitTimeMs += 10;
		}
		LOG(INFO) << "Messages received: " << receiver.messagesReceived();
		LOG(INFO) << "Correct Order: " << receiver.correctOrder();
	}

	interComm->send("Master", new InterCommNs::IntMessage(receiver.messagesReceived()));
	// Wait some time, to allow the slave to send to the master
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	InterCommNs::InterCommFactory::destroyInterComm(interComm);
//	std::cout << "Slave2 done" << std::endl;
	return 0;
}


class CrashMessageReceiver: public InterCommNs::MessageReceiverIf
{
public:
	CrashMessageReceiver(int crashMessageCount):
        mReceivedMessages(),
        mCrashMessageCount(crashMessageCount),
        mDataMutex()
	{
	};

	void receiveMessage(const InterCommNs::MessageIf* message)
	{
        std::lock_guard<std::mutex> lg (mDataMutex);
        mReceivedMessages++;
        if (mReceivedMessages >= mCrashMessageCount)
        {
            LOG(ERROR) << "Reached crash message count: " << mReceivedMessages;
            LOG(ERROR) << "Bye Bye ....";
            google::FlushLogFiles(google::INFO);
            int val2 = 1;
            val2 -= 1;

            //division by zero :-)
            int result = 100 / val2;

            LOG(ERROR) << "Should not be logged ....";

        }
	}

	int messagesReceived()
	{
        std::lock_guard<std::mutex> lg (mDataMutex);
        return mReceivedMessages;
	}

private:
    int mReceivedMessages;
    int mCrashMessageCount;
    std::mutex mDataMutex;
};

int crashwhilereceiving(int messageCount)
{
	google::InitGoogleLogging("CrashWhileReceiving");

	CrashMessageReceiver crashReceiver(messageCount);
	InterCommNs::InterCommIf* interComm = InterCommNs::InterCommFactory::createInterComm("Slave");
	interComm->createMailbox("Slave");
	interComm->registerMailbox("Master");
    LOG(INFO) << "Crash While receiving app started";

    InterCommNs::MessageIf* message = new InterCommNs::StringMessage("Slave Started");
    message->messageType("App.Started");
    interComm->send("Master", message);
    //Give us some time to send the message
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    LOG(INFO) << "Register Crash receiver";
	interComm->registerNotificationMessageType(&crashReceiver, "");

    while (crashReceiver.messagesReceived() < messageCount)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    message = new InterCommNs::StringMessage("Slave Done");
    message->messageType("App.Done");
    interComm->send("Master", message);

	InterCommNs::InterCommFactory::destroyInterComm(interComm);
	return 0;
}

int crashwhilesending(int messageCount)
{
	google::InitGoogleLogging("CrashWhileSending");

	CrashMessageReceiver crashReceiver(messageCount);
	InterCommNs::InterCommIf* interComm = InterCommNs::InterCommFactory::createInterComm("Slave");
	interComm->createMailbox("Slave");
	interComm->registerMailbox("Master");
    LOG(INFO) << "Crash While sending app started";

    //Wait some time before sending messages
    std::this_thread::sleep_for(std::chrono::seconds(3));

    InterCommNs::MessageIf* message = new InterCommNs::StringMessage("Slave Started");
    message->messageType("App.Started");
    interComm->send("Master", message);

    int totalMessages = messageCount != 0 ? messageCount : 20;
    LOG(INFO) << "Sending " << totalMessages << " messages";

    for(int i = 0; i < totalMessages; ++i)
    {
        LOG(INFO) << "Sending message: " << i;
        //Crash after the 10th message
        if (messageCount == 0 && i == 10)
        {
            LOG(ERROR) << "App should crash on this message";
            message = new CrashMessage(true);
        }
        else
        {
            message = new CrashMessage(false);
        }

        message->messageType("App.CrashMessage");
        interComm->send("Master", message);

        //Allow some time for actually sending
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

	InterCommNs::InterCommFactory::destroyInterComm(interComm);
	return 0;
}

GTEST_API_ int main(int argc, char **argv)
{
	mainApplication = std::string(argv[0]);
    testing::InitGoogleTest(&argc, argv);
	google::ParseCommandLineFlags(&argc, &argv, true);

	if (FLAGS_slave1)
	{
		return unitTestSlave1();
	}
	if (FLAGS_slave2)
	{
		return unitTestSlave2();
	}
	if (FLAGS_crashwhilereceiving && FLAGS_messagecount > 0)
	{
        return crashwhilereceiving(FLAGS_messagecount);
	}
	if (FLAGS_crashwhilesending)
	{
        return crashwhilesending(FLAGS_messagecount);
	}

    google::InitGoogleLogging("InterCommUnitTest");

	if (testing::GTEST_FLAG(filter) == "*")
	{
		if (FLAGS_quickTests)
		{
			testing::GTEST_FLAG(filter) = "-InterCommAsyncTest.MultipleSendersAtTheSameTime2";
		}
		if (FLAGS_fullTests)
		{
			testing::GTEST_FLAG(filter) = "*";
		}
	}

	LOG(INFO) << "Run normal Intercomm tests";
    int result = RUN_ALL_TESTS();

	google::ShutdownGoogleLogging();
	google::ShutDownCommandLineFlags();
	return result;
}


