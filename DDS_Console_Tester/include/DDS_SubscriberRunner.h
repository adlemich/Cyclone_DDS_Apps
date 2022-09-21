#pragma once

#ifndef DDS_SUBSCRIBERRUNNER_H
#define DDS_SUBSCRIBERRUNNER_H

#include "HelloWorldData.h"
/*
*/
// -------------------------------------------------------------------------------------------------

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
		void setParams(AutoPtr<Logger> pLogger, dds_entity_t ddsParticipant, dds_entity_t ddsTopic);
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

		// DDS
		dds_entity_t mDDSParticipant;
		dds_entity_t mDDSTopic;
		dds_entity_t mDDSReader;

};

#endif //DDS_SUBSCRIBERRUNNER_H
