#pragma once

#ifndef DDS_PINGPONGHANDLER_H
#define DDS_PINGPONGHANDLER_H

// DDS Generated code from IDL
#include "PingData.h"
#include "PongData.h"

// -------------------------------------------------------------------------------------------------

/* HELPER STRUCTS
*/
struct DDS_PONG_RESULT {
	string	sResponderHost;
	bool	bPayloadMatch;
	double	dResponseLatency_ms;

	DDS_PONG_RESULT() {
		sResponderHost = "";
		bPayloadMatch = false;
		dResponseLatency_ms = 0.0;
	}
};

struct DDS_PING_RECORD {
	Int32	  iPingID;
	string    sSourceHost;
	string    sPayload;
	Timestamp tSendingTime;
	vector<DDS_PONG_RESULT> vResultSet;
	double    dResponseLatencyAvg_ms;

	DDS_PING_RECORD() {
		iPingID = 0;
		sSourceHost = "";
		sPayload = "";
		dResponseLatencyAvg_ms = 0;
	}
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class DDS_PingPongHandler
{
	// CONSTRUCTOR - DESTRUCTOR		
	public:
		DDS_PingPongHandler(AutoPtr<Logger> pLogg);
		~DDS_PingPongHandler();

	// public Methods
	public:
		APP_RET_VAL sendPing(const Int32& repeatNr, const string& sPayload);

		APP_RET_VAL getTopicInfo(dds_entity_t& outDDSParticipant, dds_entity_t& outTopicPing, dds_entity_t& outTopicPong);
		APP_RET_VAL onPing(PingData_PingMsg* pPingData);
		APP_RET_VAL onPong(PongData_PongMsg* pPongData);

		APP_RET_VAL startPingListener();
		APP_RET_VAL stopPingListener();
		APP_RET_VAL startPongListener();
		APP_RET_VAL stopPongListener();

	// protected Methods
	protected:
		APP_RET_VAL publishMsg_Pong(const Int32& iOrigPingID, const string& sPayload);
		APP_RET_VAL createTopicWriter_Pong();
		APP_RET_VAL createTopicWriter_Ping();


		// private members
	private:
		/*The logger reference we want to use*/
		AutoPtr<Logger>		m_Logger;

		dds_entity_t mDDSParticipant;
		dds_entity_t mDDSTopicPing;
		dds_entity_t mDDSTopicPong;
		dds_entity_t mDDSWriterPing;
		dds_entity_t mDDSWriterPong;

		// Subscriber Threads and instances
		Mutex				 mMutexTopicInfo;
		Thread			     mListenerThreadPing;
		DDS_SubscriberRunner mDDSListenerPing;
		Thread			     mListenerThreadPong;
		DDS_SubscriberRunner mDDSListenerPong;

		// Ping Pong data, holding only one for the last set
		std::map<Int32, DDS_PING_RECORD> mMapSentPings;
		Int32           miPingIDIndex;
		Int32           miPongIDIndex;
};

#endif //DDS_PINGPONGHANDLER_H
