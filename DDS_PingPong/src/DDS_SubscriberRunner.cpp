
#include "App_Includes.h"
#include "App_Definitions.h"
#include "DDS_SubscriberRunner.h"
#include "DDS_PingPongHandler.h"

//############################################################################

//* CONSTRUCTOR **/
DDS_SubscriberRunner::DDS_SubscriberRunner() {
    
    mbShutdown = false;
    mpMessageHandler = NULL;
    mbListenToPing = true;
    mDDSParticipant = -1;
    mDDSTopicPing = -1;
    mDDSTopicPong = -1;
    mDDSReaderPing = -1;
    mDDSReaderPong = -1;

} //--------------------------------------------------------------------------------------------------------

//* DESTRUCTOR **/
DDS_SubscriberRunner::~DDS_SubscriberRunner() {
    poco_debug(*m_Logger, "-> DDS_SubscriberRunner::~DDS_SubscriberRunner()");

    // Delete readers
    if (mDDSReaderPing > 0) {
    
        dds_return_t rc = dds_delete(mDDSReaderPing);
        if (rc != DDS_RETCODE_OK)
            poco_error_f(*m_Logger, "ERROR while deleting DDS Reader Ping: dds_delete: %s", dds_strretcode(-rc));
    }
    if (mDDSReaderPong > 0) {

        dds_return_t rc = dds_delete(mDDSReaderPong);
        if (rc != DDS_RETCODE_OK)
            poco_error_f(*m_Logger, "ERROR while deleting DDS Reader Pong: dds_delete: %s", dds_strretcode(-rc));
    }
    
    poco_debug(*m_Logger, "<- DDS_SubscriberRunner::~DDS_SubscriberRunner()");

} //--------------------------------------------------------------------------------------------------------

void DDS_SubscriberRunner::setParams(AutoPtr<Logger> pLogger, DDS_PingPongHandler* pMessageHandler, const bool& bListenToPing) {
    // Set members
    m_Logger = pLogger;
    
    poco_debug(*m_Logger, "-> DDS_SubscriberRunner::setParams()");
    
    mpMessageHandler = pMessageHandler;
    if (mpMessageHandler == NULL) {
        poco_error(*m_Logger, "ERROR while preparing DDS Reader thread, supplied message handler instance is NULL!!");
        return;
    }
    mbListenToPing = bListenToPing;
    mpMessageHandler->getTopicInfo(mDDSParticipant, mDDSTopicPing, mDDSTopicPong);
            
    if (mDDSParticipant > 0 && mDDSTopicPing > 0 && mDDSTopicPong > 0) {
        dds_qos_t* qos;
        qos = dds_create_qos();
        dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));

        if (mbListenToPing == true) {
            mDDSReaderPing = dds_create_reader(mDDSParticipant, mDDSTopicPing, qos, NULL);
            if (mDDSReaderPing < 0) {
                poco_debug_f(*m_Logger, "ERROR while creating DDS reader for topic PING. dds_create_reader: %s", dds_strretcode(-mDDSReaderPing));
            }
            else {
                poco_debug(*m_Logger, "Creating DDS Reader for Topic PING OK.");
            }
        }
        else {
            mDDSReaderPong = dds_create_reader(mDDSParticipant, mDDSTopicPong, qos, NULL);
            if (mDDSReaderPong < 0) {
                poco_debug_f(*m_Logger, "ERROR while creating DDS reader for topic PONG. dds_create_reader: %s", dds_strretcode(-mDDSReaderPong));
            }
            else {
                poco_debug(*m_Logger, "Creating DDS Reader for Topic PONG OK.");
            }
        }
        dds_delete_qos(qos);
    }
    else {
        poco_error(*m_Logger, "ERROR while preparing DDS Reader thread, supplied DDS participant or topic IDs are invalid!!");
    }

    poco_debug(*m_Logger, "<- DDS_SubscriberRunner::setParams()");
} //--------------------------------------------------------------------------------------------------------

void DDS_SubscriberRunner::shutdownRunner() {
    mShutdownMutex.lock();
    this->mbShutdown = true;
    mShutdownMutex.unlock();
} //--------------------------------------------------------------------------------------------------------

bool DDS_SubscriberRunner::mustShutdown() {
    bool retVal;
    mShutdownMutex.lock();
    retVal = this->mbShutdown;
    mShutdownMutex.unlock();
    return retVal;
} //--------------------------------------------------------------------------------------------------------

/* Runner main function.*/
void DDS_SubscriberRunner::run() {

    poco_debug(*m_Logger, "-> DDS_SubscriberRunner::run()");
    
    dds_return_t rc;
    bool bGotData = false;

    // Ping Data
    const Int32 iMaxPingSamples = 16;
    PingData_PingMsg* pPingData;
    void* pingSamples[iMaxPingSamples];
    dds_sample_info_t pingInfos[iMaxPingSamples];
    
    for (Int32 i = 0; i < iMaxPingSamples; i++) {
        pingSamples[i] = PingData_PingMsg__alloc();
    }

    // Pong Data
    const Int32 iMaxPongSamples = 16;
    PongData_PongMsg* pPongData;
    void* pongSamples[iMaxPongSamples];
    dds_sample_info_t pongInfos[iMaxPongSamples];

    for (Int32 i = 0; i < iMaxPongSamples; i++) {
        pongSamples[i] = PongData_PongMsg__alloc();
    }

    // CHECK FOR INCOMING DATA
    if (mDDSTopicPing > 0 && mDDSTopicPong > 0) {

        poco_debug(*m_Logger, "DDS Subscriber is waiting for a sample ...");

        /* Poll until data has been read. */
        while (mustShutdown() == false)
        {
            rc = -1;
            bGotData = false;

            if (mbListenToPing == true) {
                /* Do the actual read on topic Ping. Take will remove the sample from the DDS cache right away.
                 * The return value contains the number of read samples. */
                rc = dds_take(mDDSReaderPing, pingSamples, pingInfos, iMaxPingSamples, iMaxPingSamples);
                if (rc < 0) {
                    poco_debug_f(*m_Logger, "Error while reading data from DDS topic [PING]. dds_read: %s", dds_strretcode(-rc));
                }
                else if (rc > 0) {
                    for (Int32 sI = 0; sI < rc; sI++) {
                        /* Check if we read some data and it is valid. */
                        if (pingInfos[sI].valid_data && pingInfos[sI].sample_state == DDS_NOT_READ_SAMPLE_STATE) {
                            /* Print Message. */
                            pPingData = (PingData_PingMsg*)pingSamples[sI];
                            poco_information_f(*m_Logger, "DDS RECEIVED MESSAGE on Topic [PING] <- <ID = [%d], SourceHost = [%s], Payload = [%s]>",
                                pPingData->lPingID, string(pPingData->sSourceHost), string(pPingData->sPayload));

                            // Call handler class
                            if (mpMessageHandler != NULL) {
                                mpMessageHandler->onPing(pPingData);
                            }
                            bGotData = true;
                        }
                    }
                }
            }
            else {
                /* Do the actual read on topic Pong. Take will remove the sample from the DDS cache right away.
                 * The return value contains the number of read samples. */
                rc = dds_take(mDDSReaderPong, pongSamples, pongInfos, iMaxPongSamples, iMaxPongSamples);
                if (rc < 0) {
                    poco_debug_f(*m_Logger, "Error while reading data from DDS topic [PONG]. dds_read: %s", dds_strretcode(-rc));
                }
                else if (rc > 0) {
                    /* Check if we read some data and it is valid. */
                    for (Int32 sI = 0; sI < rc; sI++) {
                        if (pongInfos[sI].valid_data == true && pongInfos[sI].sample_state == DDS_NOT_READ_SAMPLE_STATE) {
                            /* Print Message. */
                            pPongData = (PongData_PongMsg*)pongSamples[sI];
                            poco_information_f(*m_Logger, "DDS RECEIVED MESSAGE on Topic [PONG] <- <PongID = [%d], original PingID = [%d], ResponderHost = [%s], Payload = [%s]>",
                                pPongData->lPongID, pPongData->lOriPingID, string(pPongData->sResponderHost), string(pPongData->sPayload));

                            // Call handler class
                            if (mpMessageHandler != NULL) {
                                mpMessageHandler->onPong(pPongData);
                            }
                            bGotData = true;
                        }
                    }
                }
            }
            if (bGotData == false) {
                /* Polling sleep if there was no data. */
                dds_sleepfor(DDS_MSECS(1));
            } 
        }        
    }    

    // Free the sample buffer
    for (Int32 i = 0; i < iMaxPingSamples; i++) {
        PingData_PingMsg_free(pingSamples[i], DDS_FREE_ALL);
    }
    for (Int32 i = 0; i < iMaxPongSamples; i++) {
        PongData_PongMsg_free(pongSamples[i], DDS_FREE_ALL);
    }

    poco_debug(*m_Logger, "<- DDS_SubscriberRunner::run()");
} //--------------------------------------------------------------------------------------------------------
