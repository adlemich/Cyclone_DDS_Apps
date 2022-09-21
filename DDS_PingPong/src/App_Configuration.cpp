
#include "App_Includes.h"
#include "App_Definitions.h"
#include "App_Configuration.h"

//############################################################################

//* CONSTRUCTOR **/
App_Configuration::App_Configuration() {
    mbIsConfigOK = false;
} // -------------------------------------------------------------------------

//* DESTRUCTOR **/
App_Configuration::~App_Configuration() {

} // -------------------------------------------------------------------------

void App_Configuration::setLogger(AutoPtr<Logger> pLogg) {
    m_Logger = pLogg;
} // -------------------------------------------------------------------------

APP_RET_VAL	App_Configuration::setConfigFilePath(const string sFilePath) {
    APP_RET_VAL retVal = AppRetVal_OK;

    if (sFilePath.length() < 2) {
        retVal = AppRetVal_BadParam;
    }
    else {
        msConfigFilePath = sFilePath;
    }

    return retVal;
} // -------------------------------------------------------------------------

APP_RET_VAL	App_Configuration::loadConfiguration() {

    APP_RET_VAL retVal = AppRetVal_OK;

    try {
        // Check if file exists
        File configFile(msConfigFilePath);
        if (configFile.isFile() == true && configFile.exists() && configFile.canRead()) {

            // load JSON config
            mJsonConfProvider.load(msConfigFilePath);
            mbIsConfigOK = true;
        }
        else {
            printf("ERROR while loading configuration file [%s]. Please make sure the file in the run path of the application and readable as JSON.\n", msConfigFilePath.c_str());
            retVal = AppRetVal_Error;
        }
    }
    catch (Poco::Exception& exc) {
        printf("ERROR while loading configuration file [%s]. Please make sure the file in the run path of the application. \n%s.\n", msConfigFilePath.c_str(), exc.displayText().c_str());
        retVal = AppRetVal_Exception;
    }

    return retVal;
	
} // -------------------------------------------------------------------------

APP_RET_VAL	App_Configuration::dumpConfigToLog() {

    poco_debug(*m_Logger, "-> App_Configuration::dumpConfigToLog()");
    APP_RET_VAL retVal = AppRetVal_OK;

    poco_information(*m_Logger, "+++++++++++++++++++++ DUMPING ACTIVE CONFIGURATION +++++++++++++++++++");

    // Application Config
    APP_CONFIG_APPLICATION appConf;
    retVal = getApplicationConf(appConf);

    poco_information(*m_Logger, "+++ Application Config");
    poco_information_f(*m_Logger, "+++++ %s = %s", appConf.sKey_Name, appConf.sName);
    poco_information_f(*m_Logger, "+++++ %s = %s", appConf.sKey_Version, appConf.sVersion);

    // Logging Config
    APP_CONFIG_LOGGING logConf;
    retVal = getLoggingConf(logConf);

    poco_information(*m_Logger, "+++ Logging Config");
    poco_information_f(*m_Logger, "+++++ %s = %s", logConf.sKey_LogLevel, logConf.sLogLevel);
    poco_information_f(*m_Logger, "+++++ %s = %s", logConf.sKey_EnableConsoleLogging, logConf.bEnableConsoleLogging ? string("true") : string("false"));
    poco_information_f(*m_Logger, "+++++ %s = %s", logConf.sKey_EnableFileLogging, logConf.bEnableFileLogging ? string("true") : string("false"));
    poco_information_f(*m_Logger, "+++++ %s = %s", logConf.sKey_LogfileFolder, logConf.sLogfileFolder);
    poco_information_f(*m_Logger, "+++++ %s = %d", logConf.sKey_LogfileMaxMB, logConf.iLogfileMaxMB);
    poco_information_f(*m_Logger, "+++++ %s = %d", logConf.sKey_LogfileMaxFiles, logConf.iLogfileMaxFiles);

    poco_information(*m_Logger, "+++++++++++++++++++++ END OF ACTIVE CONFIGURATION +++++++++++++++++++++");


    poco_debug_f(*m_Logger, "<- App_Configuration::dumpConfigToLog(), RetVal [%d]", retVal);
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL	App_Configuration::getNumericParam(Int32& outValue, const string sKey) {

    poco_debug_f(*m_Logger, "-> App_Configuration::getNumericParam(Key = [%s])", sKey);
    APP_RET_VAL retVal = AppRetVal_OK;

    try {
        outValue = this->mJsonConfProvider.getInt(sKey);
    }
    catch (Poco::Exception& exc) {
        poco_error_f(*m_Logger,
            "ERROR while reading key [%s] from configuration file [%s]. %s.",
            msConfigFilePath, sKey, exc.displayText());
        retVal = AppRetVal_Exception;
    }

    poco_debug_f(*m_Logger, "<- App_Configuration::getNumericParam(), RetVal [%d], Value [%d]", retVal, outValue);
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL App_Configuration::getStringParam(string& outValue, const string sKey) {

    poco_debug_f(*m_Logger, "-> App_Configuration::getStringParam(Key = [%s])", sKey);
    APP_RET_VAL retVal = AppRetVal_OK;

    try {
        outValue = this->mJsonConfProvider.getString(sKey);
    }
    catch (Poco::Exception& exc) {
        poco_error_f(*m_Logger,
            "ERROR while reading key [%s] from configuration file [%s]. %s.",
            msConfigFilePath, sKey, exc.displayText());
        retVal = AppRetVal_Exception;
    }

    poco_debug_f(*m_Logger, "<- App_Configuration::getStringParam(), RetVal [%d], Value [%s]", retVal, outValue);
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL App_Configuration::getApplicationConf(APP_CONFIG_APPLICATION& outStruct) {

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mbIsConfigOK == true) {
        try {
            outStruct.sName    = mJsonConfProvider.getString(outStruct.sKey_Name);
            outStruct.sVersion = mJsonConfProvider.getString(outStruct.sKey_Version);
        }
        catch (Poco::Exception& exc) {
            printf("ERROR while reading Application configuration from configuration file [%s].\n%s.\n",msConfigFilePath.c_str(), exc.displayText().c_str());
            retVal = AppRetVal_Exception;
        }
    }
    else {
        retVal = AppRetVal_Error;
    }
    return retVal;

} // -------------------------------------------------------------------------

APP_RET_VAL App_Configuration::getLoggingConf(APP_CONFIG_LOGGING& outStruct) {

    APP_RET_VAL retVal = AppRetVal_OK;

    if (mbIsConfigOK == true) {
        try {
            outStruct.sLogLevel             = mJsonConfProvider.getString(outStruct.sKey_LogLevel);
            outStruct.sLogfileFolder        = mJsonConfProvider.getString(outStruct.sKey_LogfileFolder);
            outStruct.bEnableConsoleLogging = mJsonConfProvider.getBool(outStruct.sKey_EnableConsoleLogging);
            outStruct.bEnableFileLogging    = mJsonConfProvider.getBool(outStruct.sKey_EnableFileLogging);
            outStruct.iLogfileMaxMB         = mJsonConfProvider.getInt(outStruct.sKey_LogfileMaxMB);
            outStruct.iLogfileMaxFiles      = mJsonConfProvider.getInt(outStruct.sKey_LogfileMaxFiles);
        }
        catch (Poco::Exception& exc) {
            printf("ERROR while reading Logging configuration from configuration file [%s]. \n%s.\n", msConfigFilePath.c_str(), exc.displayText().c_str());
            retVal = AppRetVal_Exception;
        }
    }
    else {
        retVal = AppRetVal_Error;
    }
    return retVal;

}// -------------------------------------------------------------------------