#pragma once

#ifndef DDS_SUBSCRIBERRUNNER_H
#define DDS_SUBSCRIBERRUNNER_H

#include "HelloWorldData.h"
/*
*/
// -------------------------------------------------------------------------------------------------

// forward declaration
class DDS_PingPongHandler;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class DDS_SubscriberRunner : public Runnable
{
	// CONSTRUCTOR - DESTRUCTOR		
	public:
		DDS_SubscriberRunner();
		~DDS_SubscriberRunner();

	// public Methods
	public:
		void setParams(AutoPtr<Logger> pLogger, DDS_PingPongHandler* pMessageHandler, const bool& bListenToPing);
		virtual void run();
		void shutdownRunner();

	protected:
		bool mustShutdown();
		
		// private members
	private:
		/*The logger reference we want to use*/
		AutoPtr<Logger>		m_Logger;
		bool				mbShutdown;
		Mutex               mShutdownMutex;
		bool				mbListenToPing;

		// Message Handler
		DDS_PingPongHandler* mpMessageHandler;

		// DDS
		dds_entity_t mDDSParticipant;
		dds_entity_t mDDSTopicPing;
		dds_entity_t mDDSTopicPong;
		dds_entity_t mDDSReaderPing;
		dds_entity_t mDDSReaderPong;

};

#endif //DDS_SUBSCRIBERRUNNER_H
