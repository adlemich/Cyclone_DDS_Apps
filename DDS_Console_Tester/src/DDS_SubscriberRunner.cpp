
#include "App_Includes.h"
#include "App_Definitions.h"
#include "DDS_SubscriberRunner.h"

//############################################################################

//* CONSTRUCTOR **/
DDS_SubscriberRunner::DDS_SubscriberRunner() {
    mDDSParticipant = -1;
    mDDSTopic = -1;
    mDDSReader = -1;
    mbShutdown = false;
} //--------------------------------------------------------------------------------------------------------

//* DESTRUCTOR **/
DDS_SubscriberRunner::~DDS_SubscriberRunner() {
    poco_debug(*m_Logger, "-> DDS_SubscriberRunner::~DDS_SubscriberRunner()");

    if (mDDSReader > 0) {
    
        dds_return_t rc = dds_delete(mDDSReader);
        if (rc != DDS_RETCODE_OK)
            poco_error_f(*m_Logger, "ERROR while deleting DDS Reader: dds_delete: %s", dds_strretcode(-rc));
    }
    
    poco_debug(*m_Logger, "<- DDS_SubscriberRunner::~DDS_SubscriberRunner()");

} //--------------------------------------------------------------------------------------------------------

void DDS_SubscriberRunner::setParams(AutoPtr<Logger> pLogger, dds_entity_t ddsParticipant, dds_entity_t ddsTopic) {
    // Set members
    m_Logger = pLogger;
    
    poco_debug_f(*m_Logger, "-> DDS_SubscriberRunner::setParams(ddsParticipant = [%d]), ddsTopic = [%d]", ddsParticipant, ddsTopic);
    
    mDDSParticipant = ddsParticipant;
    mDDSTopic = ddsTopic;

    if (mDDSParticipant > 0 && mDDSTopic > 0) {

        /* Create a reliable Reader. */
        dds_qos_t* qos;
        qos = dds_create_qos();
        dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));

        mDDSReader = dds_create_reader(mDDSParticipant, mDDSTopic, qos, NULL);
        if (mDDSReader < 0) {
            poco_debug_f(*m_Logger, "ERROR while creating DDS reader. dds_create_reader: %s", dds_strretcode(-mDDSReader));
        }
        else {
            poco_debug(*m_Logger, "Creating DDS Reader OK.");
        }
        dds_delete_qos(qos);
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
    
    const Int32 iMaxSamples = 16;
    HelloWorldData_Msg* msg;
    void* samples[iMaxSamples];
    dds_sample_info_t infos[iMaxSamples];
    dds_return_t rc;
   
    if (mDDSReader > 0) {

        poco_debug(*m_Logger, "DDS Subscriber is waiting for a sample ...");
       
        /* Initialize sample buffer, by pointing the void pointer within
         * the buffer array to a valid sample memory location. */
        for (int i = 0; i < iMaxSamples; i++) {
            samples[i] = HelloWorldData_Msg__alloc();
        }

        /* Poll until data has been read. */
        while (mustShutdown() == false)
        {
            /* Do the actual read.
             * The return value contains the number of read samples. */
            rc = dds_read(mDDSReader, samples, infos, iMaxSamples, iMaxSamples);
            if (rc < 0)
                poco_debug_f(*m_Logger, "Error while reading data from DDS topic. dds_read: %s", dds_strretcode(-rc));

            /* Check if we read some data and it is valid. */
            if ((rc > 0) && (infos[0].valid_data))
            {
                /* Print Message. */
                msg = (HelloWorldData_Msg*)samples[0];
                poco_information_f(*m_Logger, "DDS RECEIVED MESSAGE <- <ID = [%d], Size = [%d] ,Message = [%s]>", msg->msgID, msg->msgSize, string(msg->message));
                break;
            }
            else
            {
                /* Polling sleep. */
                dds_sleepfor(DDS_MSECS(20));
            }
        }

        // Free the sample buffer
        for (int i = 0; i < iMaxSamples; i++) {
            HelloWorldData_Msg_free(samples[i], DDS_FREE_ALL);
        }
    }       

    poco_debug(*m_Logger, "<- DDS_SubscriberRunner::run()");
} //--------------------------------------------------------------------------------------------------------
