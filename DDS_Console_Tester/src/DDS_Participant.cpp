
#include "App_Includes.h"
#include "App_Definitions.h"
#include "DDS_SubscriberRunner.h"
#include "DDS_Participant.h"

//############################################################################

//* CONSTRUCTOR **/
DDS_Participant::DDS_Participant(AutoPtr<Logger> pLogg) {

    // Set members
    m_Logger = pLogg;
    mDDSParticipant = -1;
    mDDSTopicHelloWorld = -1;
    mDDSWriterHelloWorld = -1;
    mDDSTopicPing = -1;
    mDDSTopicPong = -1;
    mDDSWriterPing = -1;
    mDDSWriterPong = -1;

    /* Create a Participant. */
    mDDSParticipant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
    if (mDDSParticipant < 0)
        poco_error_f(*m_Logger, "dds_create_participant: %s", dds_strretcode(-mDDSParticipant));

    // Create all Topic Writers
    createTopicWriter_HelloWorld();
    createTopicWriter_Pong();
    createTopicWriter_Ping();
    
} // -------------------------------------------------------------------------

//* DESTRUCTOR **/
DDS_Participant::~DDS_Participant() {
    poco_debug(*m_Logger, "-> DDS_Participant::~DDS_Participant()");

    /* Deleting the participant will delete all its children recursively as well. */
    dds_return_t rc;
    rc = dds_delete(mDDSParticipant);
    if (rc != DDS_RETCODE_OK)
        poco_error_f(*m_Logger, "dds_delete: %s\n", dds_strretcode(-rc));
    
    poco_debug(*m_Logger, "<- DDS_Participant::~DDS_Participant()");

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::createTopicWriter_HelloWorld() {
    poco_debug(*m_Logger, "-> DDS_Participant::createTopicWriter_HelloWorld()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mDDSParticipant < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_HelloWorld(): DDSParticipant not initalized! Can not create topic.");
        retVal = AppRetVal_Error;
    }
    else {

        /* Create a Topic. */
        mDDSTopicHelloWorld = dds_create_topic(mDDSParticipant, &HelloWorldData_Msg_desc, "HelloWorldData_Msg", NULL, NULL);
        if (mDDSTopicHelloWorld < 0) {
            poco_error_f(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_HelloWorld(): dds_create_topic failed: %s", dds_strretcode(-mDDSTopicHelloWorld));
        }
        else {
            poco_debug(*m_Logger, "Creating Topic [HelloWorld] OK!");
        }

        /* Create a Writer. */
        mDDSWriterHelloWorld = dds_create_writer(mDDSParticipant, mDDSTopicHelloWorld, NULL, NULL);
        if (mDDSWriterHelloWorld < 0) {
            poco_error_f(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_HelloWorld(): dds_create_writer failed: %s", dds_strretcode(-mDDSWriterHelloWorld));
        }
        else {
            poco_debug(*m_Logger, "Creating Writer for topic [HelloWorld] OK!");
        }
    }

    poco_debug(*m_Logger, "<- DDS_Participant::createTopicWriter_HelloWorld()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::createTopicWriter_Pong() {
    poco_debug(*m_Logger, "-> DDS_Participant::createTopicWriter_Pong()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mDDSParticipant < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_Pong(): DDSParticipant not initalized! Can not create topic.");
        retVal = AppRetVal_Error;
    }
    else {

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
    }

    poco_debug(*m_Logger, "<- DDS_Participant::createTopicWriter_Pong()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::createTopicWriter_Ping() {
    poco_debug(*m_Logger, "-> DDS_Participant::createTopicWriter_Ping()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mDDSParticipant < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::createTopicWriter_Ping(): DDSParticipant not initalized! Can not create topic.");
        retVal = AppRetVal_Error;
    }
    else {

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
    }

    poco_debug(*m_Logger, "<- DDS_Participant::createTopicWriter_Ping()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::publishMsg_HelloWorld(const Int32& iMsgID, const string& sMsg) {
    poco_debug_f(*m_Logger, "-> DDS_Participant::publishMsg_HelloWorld(iMsgID = [%d], sMsg = [%s])", iMsgID, sMsg);

    APP_RET_VAL retVal = AppRetVal_OK;
    dds_return_t rc;
    HelloWorldData_Msg* pMsgPack = nullptr;

    if (mDDSParticipant < 0 || mDDSTopicHelloWorld < 0 || mDDSWriterHelloWorld < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::publishMsg_HelloWorld(): DDS system not initalized! Can not send message.");
        retVal = AppRetVal_Error;
    }
    else {
        
        HelloWorldData_Msg msg;
        msg.msgID = iMsgID;
        msg.msgSize = (int32_t)sMsg.length();
        msg.message = (char*)malloc(msg.msgSize + 1);
        if (msg.message != NULL) {
            memset(msg.message, 0, msg.msgSize + 1);
            sMsg.copy(msg.message, msg.msgSize, 0);
        }

        // Send stuff
        rc = dds_write(mDDSWriterHelloWorld, &msg);
        if (rc != DDS_RETCODE_OK) {
            poco_error_f(*m_Logger, "ERROR in DDS_Participant::publishMsg_HelloWorld(): dds_write failed: %s", dds_strretcode(-rc));
        }
        else {
            poco_debug_f(*m_Logger, "DDS PUBLISH ok -> Topic [HelloWorld] (iMsgID = [%d], sMsg = [%s])", iMsgID, sMsg);
        }

        // Free memory
        if (msg.message != NULL) {
            free(msg.message);
        }
    }

    poco_debug(*m_Logger, "<- DDS_Participant::publishMsg_HelloWorld()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::publishMsg_Ping(const Int32& iPingID, const string& sPayload) {
    poco_debug_f(*m_Logger, "-> DDS_Participant::publishMsg_Ping(iPingID = [%d], sPayload = [%s])", iPingID, sPayload);

    APP_RET_VAL retVal = AppRetVal_OK;
    dds_return_t rc;
    PingData_PingMsg* pMsgPack = nullptr;
    size_t stringSize = 0;

    if (mDDSParticipant < 0 || mDDSTopicPing < 0 || mDDSWriterPing < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::publishMsg_Ping(): DDS system not initalized! Can not send message.");
        retVal = AppRetVal_Error;
    }
    else {

        PingData_PingMsg msg{};
        msg.lPingID = iPingID;
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
        rc = dds_write(mDDSWriterPing, &msg);
        if (rc != DDS_RETCODE_OK) {
            poco_error_f(*m_Logger, "ERROR in DDS_Participant::publishMsg_Ping(): dds_write failed: %s", dds_strretcode(-rc));
        }
        else {
            poco_debug_f(*m_Logger, "DDS PUBLISH ok -> Topic [Ping] (iPingID = [%d], sSourceHost = [%s}, sPayload = [%s])", iPingID, Environment::nodeName(), sPayload);
        }

        // Free memory
        if (msg.sPayload != NULL) {
            free(msg.sPayload);
        }
        if (msg.sSourceHost != NULL) {
            free(msg.sSourceHost);
        }
    }

    poco_debug(*m_Logger, "<- DDS_Participant::publishMsg_Ping()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::publishMsg_Pong(const Int32& iPingID, const string& sPayload) {
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
            poco_debug_f(*m_Logger, "DDS PUBLISH ok -> Topic [Pong] (lOriPingID = [%d], sResponderHost = [%s}, sPayload = [%s])", iPingID, Environment::nodeName(), sPayload);
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

APP_RET_VAL DDS_Participant::startSubsc_HelloWorld() {
    poco_debug(*m_Logger, "-> DDS_Participant::startSubsc_HelloWorld()");

    APP_RET_VAL retVal = AppRetVal_OK;
  
    if (mDDSParticipant < 0 || mDDSTopicHelloWorld < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::startSubsc_HelloWorld(): DDS system not initalized! Can not start subscriber.");
        retVal = AppRetVal_Error;
    }
    else {
        mDDSSubscriberRunner.setParams(m_Logger, mDDSParticipant, mDDSTopicHelloWorld);
        mSubscriberThread.start(mDDSSubscriberRunner);
    }

    poco_debug(*m_Logger, "<- DDS_Participant::startSubsc_HelloWorld()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::startSubsc_Ping() {
    poco_debug(*m_Logger, "-> DDS_Participant::startSubsc_Ping()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mDDSParticipant < 0 || mDDSTopicPing < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::startSubsc_Ping(): DDS system not initalized! Can not start subscriber.");
        retVal = AppRetVal_Error;
    }
    else {
        mDDSSubscriberRunnerPing.setParams(m_Logger, mDDSParticipant, mDDSTopicPing);
        mSubscriberThreadPing.start(mDDSSubscriberRunnerPing);
    }

    poco_debug(*m_Logger, "<- DDS_Participant::startSubsc_Ping()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::startSubsc_Pong() {
    poco_debug(*m_Logger, "-> DDS_Participant::startSubsc_Pong()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mDDSParticipant < 0 || mDDSTopicPong < 0) {
        poco_error(*m_Logger, "ERROR in DDS_Participant::startSubsc_Pong(): DDS system not initalized! Can not start subscriber.");
        retVal = AppRetVal_Error;
    }
    else {
        mDDSSubscriberRunnerPong.setParams(m_Logger, mDDSParticipant, mDDSTopicPong);
        mSubscriberThreadPong.start(mDDSSubscriberRunnerPong);
    }

    poco_debug(*m_Logger, "<- DDS_Participant::startSubsc_Pong()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::stopSubsc_HelloWorld() {
    poco_debug(*m_Logger, "-> DDS_Participant::stopSubsc_HelloWorld()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mSubscriberThread.isRunning()) {
        mDDSSubscriberRunner.shutdownRunner();
        mSubscriberThread.join();
    }


    poco_debug(*m_Logger, "<- DDS_Participant::stopSubsc_HelloWorld()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::stopSubsc_Ping() {
    poco_debug(*m_Logger, "-> DDS_Participant::stopSubsc_Ping()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mSubscriberThreadPing.isRunning()) {
        mDDSSubscriberRunnerPing.shutdownRunner();
        mSubscriberThreadPing.join();
    }


    poco_debug(*m_Logger, "<- DDS_Participant::stopSubsc_Ping()");
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL DDS_Participant::stopSubsc_Pong() {
    poco_debug(*m_Logger, "-> DDS_Participant::stopSubsc_Pong()");

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mSubscriberThreadPong.isRunning()) {
        mDDSSubscriberRunnerPong.shutdownRunner();
        mSubscriberThreadPong.join();
    }


    poco_debug(*m_Logger, "<- DDS_Participant::stopSubsc_Pong()");
    return retVal;

} // -------------------------------------------------------------------------
