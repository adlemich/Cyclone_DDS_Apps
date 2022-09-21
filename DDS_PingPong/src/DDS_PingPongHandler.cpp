
#include "App_Includes.h"
#include "App_Definitions.h"
#include "DDS_SubscriberRunner.h"
#include "DDS_PingPongHandler.h"

//############################################################################

//* CONSTRUCTOR **/
DDS_PingPongHandler::DDS_PingPongHandler(AutoPtr<Logger> pLogg) {

    // Set members
    m_Logger = pLogg;
    mDDSParticipant = -1;
    mDDSTopicPing = -1;
    mDDSTopicPong = -1;
    mDDSWriterPing = -1;
    mDDSWriterPong = -1;
    mMapSentPings.clear();

    //Seeding Msg Index with some random number to allow DDS to differentiate the individual messages
    srand((unsigned int)time(NULL));
    miPingIDIndex = rand() % 1000;
    miPongIDIndex = rand() % 1000;

    /* Create a Participant. */
    mDDSParticipant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (mDDSParticipant < 0)
        poco_error_f(*m_Logger, "dds_create_participant: %s", dds_strretcode(-mDDSParticipant));

    // Create all Topic Writers
    createTopicWriter_Pong();
    createTopicWriter_Ping();

    
} // -------------------------------------------------------------------------

//* DESTRUCTOR **/
DDS_PingPongHandler::~DDS_PingPongHandler() {
    poco_debug(*m_Logger, "-> DDS_Participant::~DDS_Participant()");

    // Stop listener threads
    stopPingListener();
    stopPongListener();

    mMapSentPings.clear();

    /* Deleting the participant will delete all its children recursively as well. */
    dds_return_t rc;
    rc = dds_delete(mDDSParticipant);
    if (rc != DDS_RETCODE_OK)
        poco_error_f(*m_Logger, "dds_delete: %s\n", dds_strretcode(-rc));
    
    poco_debug(*m_Logger, "<- DDS_Participant::~DDS_Participant()");

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::createTopicWriter_Pong() {
    poco_debug(*m_Logger, "-> DDS_Participant::createTopicWriter_Pong()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mDDSParticipant < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_Pong(): DDSParticipant not initalized! Can not create topic.");
        retVal = AppRetVal_Error;
    }
    else {

        mMutexTopicInfo.lock();
        /* Create a Topic. */
        mDDSTopicPong = dds_create_topic(mDDSParticipant, &PongData_PongMsg_desc, "Pong_Msg", NULL, NULL);
        if (mDDSTopicPong < 0) {
            poco_error_f(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_Pong(): dds_create_topic failed: %s", dds_strretcode(-mDDSTopicPong));
        }
        else {
            poco_debug(*m_Logger, "Creating Topic [Pong] OK!");
        }

        /* Create a Writer. */
        mDDSWriterPong = dds_create_writer(mDDSParticipant, mDDSTopicPong, NULL, NULL);
        if (mDDSWriterPong < 0) {
            poco_error_f(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_Pong(): dds_create_writer failed: %s", dds_strretcode(-mDDSWriterPong));
        }
        else {
            poco_debug(*m_Logger, "Creating Writer for topic [Pong] OK!");
        }
        mMutexTopicInfo.unlock();
    }

    poco_debug(*m_Logger, "<- DDS_Participant::createTopicWriter_Pong()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::createTopicWriter_Ping() {
    poco_debug(*m_Logger, "-> DDS_Participant::createTopicWriter_Ping()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mDDSParticipant < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_Ping(): DDSParticipant not initalized! Can not create topic.");
        retVal = AppRetVal_Error;
    }
    else {

        mMutexTopicInfo.lock();
        /* Create a Topic. */
        mDDSTopicPing = dds_create_topic(mDDSParticipant, &PingData_PingMsg_desc, "Ping_Msg", NULL, NULL);
        if (mDDSTopicPing < 0) {
            poco_error_f(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_Ping(): dds_create_topic failed: %s", dds_strretcode(-mDDSTopicPing));
        }
        else {
            poco_debug(*m_Logger, "Creating Topic [Ping] OK!");
        }

        /* Create a Writer. */
        mDDSWriterPing = dds_create_writer(mDDSParticipant, mDDSTopicPing, NULL, NULL);
        if (mDDSWriterPing < 0) {
            poco_error_f(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_Ping(): dds_create_writer failed: %s", dds_strretcode(-mDDSWriterPing));
        }
        else {
            poco_debug(*m_Logger, "Creating Writer for topic [Ping] OK!");
        }
        mMutexTopicInfo.unlock();
    }

    poco_debug(*m_Logger, "<- DDS_Participant::createTopicWriter_Ping()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::sendPing(const Int32& repeatNr, const string& sPayload) {
    poco_debug_f(*m_Logger, "-> DDS_Participant::sendPing(repeatNr = [%d], sPayload = [%s])", repeatNr, sPayload);

    APP_RET_VAL retVal = AppRetVal_OK;
    dds_return_t rc = 0;
    PingData_PingMsg* pMsgPack = nullptr;
    size_t stringSize = 0;

    if (mDDSParticipant < 0 || mDDSTopicPing < 0 || mDDSWriterPing < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::sendPing(): DDS system not initalized! Can not send message.");
        retVal = AppRetVal_Error;
    }
    else {
       
        // Start listening for PONGs
        startPongListener();

        // Now prepare for message sending
        PingData_PingMsg msg{};        
        stringSize = sPayload.length();
        msg.sPayload = (char*)malloc(stringSize + 1);
        if (msg.sPayload != NULL) {
            memset(msg.sPayload, 0, stringSize + 1);
            sPayload.copy(msg.sPayload, stringSize, 0);
        }
        stringSize = Environment::nodeName().length();
        msg.sSourceHost = (char*)malloc(stringSize + 1);
        if (msg.sSourceHost != NULL) {
            memset(msg.sSourceHost, 0, stringSize + 1);
            Environment::nodeName().copy(msg.sSourceHost, stringSize, 0);
        }        
        // Send stuff
        poco_information(*m_Logger, "Sending PING(s) now!");

        for (Int32 iP = 0; iP < repeatNr; iP++) {
            msg.lPingID = miPingIDIndex++;

            rc = dds_write(mDDSWriterPing, &msg);
            if (rc != DDS_RETCODE_OK) {
                poco_error_f(*m_Logger, "ERROR in DDS_Participant::sendPing(): dds_write failed: %s", dds_strretcode(-rc));
            }
            else {
                poco_debug_f(*m_Logger, "DDS PUBLISH ok -> Topic [Ping] (iPingID = [%d], sSourceHost = [%s}, sPayload = [%s])", msg.lPingID, Environment::nodeName(), sPayload);
                
                DDS_PING_RECORD pingRecord;
                pingRecord.iPingID = msg.lPingID;
                pingRecord.sSourceHost = Environment::nodeName();
                pingRecord.sPayload = sPayload;
                mMapSentPings.insert(std::pair<Int32, DDS_PING_RECORD>(msg.lPingID, pingRecord));

                Thread::sleep(1000);
            }
        }

        // Free memory
        if (msg.sPayload != NULL) {
            free(msg.sPayload);
        }
        if (msg.sSourceHost != NULL) {
            free(msg.sSourceHost);
        }
    }

    poco_debug(*m_Logger, "<- DDS_Participant::sendPing()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::publishMsg_Pong(const Int32& iPingID, const string& sPayload) {
    poco_debug_f(*m_Logger, "-> DDS_Participant::publishMsg_Pong(iPongID = [%d], sPayload = [%s])", iPingID, sPayload);

    APP_RET_VAL retVal = AppRetVal_OK;
    dds_return_t rc;
    PongData_PongMsg* pMsgPack = nullptr;
    size_t stringSize = 0;

    if (mDDSParticipant < 0 || mDDSTopicPong < 0 || mDDSWriterPong < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::publishMsg_Pong(): DDS system not initalized! Can not send message.");
        retVal = AppRetVal_Error;
    }
    else {

        PongData_PongMsg msg{};
        msg.lPongID = miPongIDIndex++;
        msg.lOriPingID = iPingID;        
        stringSize = sPayload.length();
        msg.sPayload = (char*)malloc(stringSize + 1);
        if (msg.sPayload != NULL) {
            memset(msg.sPayload, 0, stringSize + 1);
            sPayload.copy(msg.sPayload, stringSize, 0);
        }
        stringSize = Environment::nodeName().length();
        msg.sResponderHost = (char*)malloc(stringSize + 1);
        if (msg.sResponderHost != NULL) {
            memset(msg.sResponderHost, 0, stringSize + 1);
            Environment::nodeName().copy(msg.sResponderHost, stringSize, 0);
        }

        // Send stuff
        rc = dds_write(mDDSWriterPong, &msg);
        if (rc != DDS_RETCODE_OK) {
            poco_error_f(*m_Logger, "ERROR in DDS_Participant::publishMsg_Pong(): dds_write failed: %s", dds_strretcode(-rc));
        }
        else {
            poco_debug_f(*m_Logger, "DDS PUBLISH ok -> Topic [Pong] (msg.lPongID = [%d], lOriPingID = [%d], sResponderHost = [%s}, sPayload = [%s])", msg.lPongID, msg.lOriPingID, Environment::nodeName(), sPayload);
        }

        // Free memory
        if (msg.sPayload != NULL) {
            free(msg.sPayload);
        }
        if (msg.sResponderHost != NULL) {
            free(msg.sResponderHost);
        }
    }

    poco_debug(*m_Logger, "<- DDS_Participant::publishMsg_Pong()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::startPingListener() {
    poco_debug(*m_Logger, "-> DDS_Participant::startPingListener()");

    APP_RET_VAL retVal = AppRetVal_OK;

    // Stop pong listener if its running
    stopPongListener();

    if (mDDSParticipant < 0 || mDDSTopicPing < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::startPingListener(): DDS system not initalized! Can not start subscriber.");
        retVal = AppRetVal_Error;
    }
    else {
        if (mListenerThreadPing.isRunning() == false) {
            mDDSListenerPing.setParams(m_Logger, this, true);
            mListenerThreadPing.start(mDDSListenerPing);
        }
    }

    poco_debug(*m_Logger, "<- DDS_Participant::startPingListener()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::stopPingListener() {
    poco_debug(*m_Logger, "-> DDS_Participant::stopPingListener()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mListenerThreadPing.isRunning()) {
        mDDSListenerPing.shutdownRunner();
        mListenerThreadPing.join();
    }

    poco_debug(*m_Logger, "<- DDS_Participant::stopPingListener()");
    return retVal;

} // ---------------------------------------------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::startPongListener() {
    poco_debug(*m_Logger, "-> DDS_Participant::startPongListener()");

    APP_RET_VAL retVal = AppRetVal_OK;

    // Stop Ping listener if its running
    stopPingListener();
  
    if (mDDSParticipant < 0 || mDDSTopicPong < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::startPongListener(): DDS system not initalized! Can not start subscriber.");
        retVal = AppRetVal_Error;
    }
    else {
        if (mListenerThreadPong.isRunning() == false) {
            mDDSListenerPong.setParams(m_Logger, this, false);
            mListenerThreadPong.start(mDDSListenerPong);
        }
    }

    poco_debug(*m_Logger, "<- DDS_Participant::startPongListener()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::stopPongListener() {
    poco_debug(*m_Logger, "-> DDS_Participant::stopPongListener()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mListenerThreadPong.isRunning()) {
        mDDSListenerPong.shutdownRunner();
        mListenerThreadPong.join();
    }

    poco_debug(*m_Logger, "<- DDS_Participant::stopPongListener()");
    return retVal;

} // ---------------------------------------------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::getTopicInfo(dds_entity_t& outDDSParticipant, dds_entity_t& outTopicPing, dds_entity_t& outTopicPong) {
    poco_debug(*m_Logger, "-> DDS_Participant::getTopicInfo()");
    APP_RET_VAL retVal = AppRetVal_OK;

    mMutexTopicInfo.lock();

    outDDSParticipant = this->mDDSParticipant;
    outTopicPing = this->mDDSTopicPing;
    outTopicPong = this->mDDSTopicPong;

    mMutexTopicInfo.unlock();

    poco_debug(*m_Logger, "<- DDS_Participant::getTopicInfo()");
    return retVal;

} // ---------------------------------------------------------------------------------------------------------------


APP_RET_VAL DDS_PingPongHandler::onPing(PingData_PingMsg* pPingData) {
    poco_debug(*m_Logger, "-> DDS_Participant::onPing()");
    APP_RET_VAL retVal = AppRetVal_OK;

    if (pPingData != NULL) {

        poco_information(*m_Logger, "We received a PING - sending PONG now!");
        // Respond with a Pong
        this->publishMsg_Pong(pPingData->lPingID, string(pPingData->sPayload));
    }
    else {
        poco_debug(*m_Logger, "ERROR while processing Ping message, parameter error (NULL Pointer)!");
        retVal = AppRetVal_BadParam;
    }

    poco_debug(*m_Logger, "<- DDS_Participant::onPing()");
    return retVal;
} // ---------------------------------------------------------------------------------------------------------------

APP_RET_VAL DDS_PingPongHandler::onPong(PongData_PongMsg* pPongData) {
    poco_debug(*m_Logger, "-> DDS_Participant::onPong()");
    APP_RET_VAL retVal = AppRetVal_OK;

    if (pPongData != NULL) {
        // Ok, now check stuff
        DDS_PONG_RESULT resultSet;
        std::map<Int32, DDS_PING_RECORD>::iterator iter = mMapSentPings.find(pPongData->lOriPingID);
        if (iter != mMapSentPings.end()) {
            
            resultSet.dResponseLatency_ms = iter->second.tSendingTime.elapsed() / 1000.0;
            resultSet.sResponderHost = string(pPongData->sResponderHost);

            if (iter->second.sPayload.compare(pPongData->sPayload) == 0) {
                resultSet.bPayloadMatch = true;
            }
            
            iter->second.vResultSet.push_back(resultSet);

            double dTotalTime_ms = 0.0;
            for (Int32 vI = 0; vI < iter->second.vResultSet.size(); vI++) {
                dTotalTime_ms += iter->second.vResultSet.at(vI).dResponseLatency_ms;
            }
            iter->second.dResponseLatencyAvg_ms = dTotalTime_ms / iter->second.vResultSet.size();

            poco_information_f(*m_Logger, "Response from [%s], Payload OK [%b], Latency = [%f] ms, Avg Latency over all resonses = [%f] ms",
                resultSet.sResponderHost, resultSet.bPayloadMatch, resultSet.dResponseLatency_ms, iter->second.dResponseLatencyAvg_ms);
            printf("Response from [%s], Payload OK [%s], Latency = [%f] ms, Avg Latency over all resonses for Ping #%d = [%f] ms\n",
                resultSet.sResponderHost.c_str(), resultSet.bPayloadMatch ? "true" : "false",
                resultSet.dResponseLatency_ms, iter->second.iPingID, iter->second.dResponseLatencyAvg_ms);
        }
        else {
            poco_debug_f(*m_Logger, "Received PONG message for Ping ID <%d>, ID was not sent by us, so ignoring it!", pPongData->lOriPingID);
        }
    }
    else {
        poco_debug(*m_Logger, "ERROR while processing Pong message, parameter error (NULL Pointer)!");
        retVal = AppRetVal_BadParam;
    }

    poco_debug(*m_Logger, "<- DDS_Participant::onPong()");
    return retVal;

} // ---------------------------------------------------------------------------------------------------------------