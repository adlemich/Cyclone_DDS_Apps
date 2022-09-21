#pragma once

#ifndef DDS_PARTICIPANT_H
#define DDS_PARTICIPANT_H

// DDS Generated code from IDL
#include "HelloWorldData.h"
#include "PingData.h"
#include "PongData.h"

/*
*/
// -------------------------------------------------------------------------------------------------

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class DDS_Participant
{
	// CONSTRUCTOR - DESTRUCTOR		
	public:
		DDS_Participant(AutoPtr<Logger> pLogg);
		~DDS_Participant();

	// public Methods
	public:
		APP_RET_VAL publishMsg_HelloWorld(const Int32& iMsgID, const string& sMsg);
		APP_RET_VAL startSubsc_HelloWorld();
		APP_RET_VAL stopSubsc_HelloWorld();

		APP_RET_VAL publishMsg_Ping(const Int32& iPingID, const string& sPayload);
		APP_RET_VAL publishMsg_Pong(const Int32& iPingID, const string& sPayload);
		APP_RET_VAL startSubsc_Ping();
		APP_RET_VAL stopSubsc_Ping();
		APP_RET_VAL startSubsc_Pong();
		APP_RET_VAL stopSubsc_Pong();

	// protected Methods
	protected:
		APP_RET_VAL createTopicWriter_HelloWorld();
		APP_RET_VAL createTopicWriter_Pong();
		APP_RET_VAL createTopicWriter_Ping();

		// private members
	private:
		/*The logger reference we want to use*/
		AutoPtr<Logger>		m_Logger;

		dds_entity_t mDDSParticipant;
		dds_entity_t mDDSTopicHelloWorld;
		dds_entity_t mDDSWriterHelloWorld;
		dds_entity_t mDDSTopicPing;
		dds_entity_t mDDSTopicPong;
		dds_entity_t mDDSWriterPing;
		dds_entity_t mDDSWriterPong;

		// Subscriber Threads and instances
		Thread					mSubscriberThread;
		DDS_SubscriberRunner	mDDSSubscriberRunner;
		Thread					mSubscriberThreadPing;
		DDS_SubscriberRunner	mDDSSubscriberRunnerPing;
		Thread					mSubscriberThreadPong;
		DDS_SubscriberRunner	mDDSSubscriberRunnerPong;
};

#endif //DDS_PARTICIPANT_H
