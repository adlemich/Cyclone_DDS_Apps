#pragma once

#ifndef APP_CONFIGURATION_H
#define APP_CONFIGURATION_H

/*

As a minimum, this application expects a JSON configuration which includes the ApplicationName and a logging block:

{
   "Application" : {
     "Name" : "Some_BlaApp",
	 "Version" : "1.0"
   },
   "Logging" : {
	 "LogLevel" : "debug",
	 "EnableConsoleLogging" : false,
	 "EnableFileLogging" : true,
	 "LogfileFolder" : "./logs",
	 "LogfileMaxMB" : 1,
	 "LogfileMaxFiles" : 3
   }
}

Additional configuration blocks can be added and retreived via the generic Get functions while using as key format  <block>.<parameter>.
*/
// -------------------------------------------------------------------------------------------------

struct APP_CONFIG_APPLICATION {
	// Keys
	string sKey_Name;
	string sKey_Version;
	// Values
	string sName;
	string sVersion;

	//Defaults
	APP_CONFIG_APPLICATION() {
		// Keys
		sKey_Name    = "Application.Name";
		sKey_Version = "Application.Version";
		// Values
		sName    = "";
		sVersion = "0.0.0";
	}
};
// -------------------------------------------------------------------------------------------------

struct APP_CONFIG_LOGGING {
	// Keys
	string sKey_LogLevel;
	string sKey_EnableConsoleLogging;
	string sKey_EnableFileLogging;
	string sKey_LogfileFolder;
	string sKey_LogfileMaxMB;
	string sKey_LogfileMaxFiles;
	// Values
	string sLogLevel;
	bool   bEnableConsoleLogging;
	bool   bEnableFileLogging;
	string sLogfileFolder;
	Int32  iLogfileMaxMB;
	Int32  iLogfileMaxFiles;

	// Defaults
	APP_CONFIG_LOGGING() {
		// Keys
		sKey_LogLevel             = "Logging.LogLevel";
		sKey_EnableConsoleLogging = "Logging.EnableConsoleLogging";
		sKey_EnableFileLogging    = "Logging.EnableFileLogging";
		sKey_LogfileFolder        = "Logging.LogfileFolder";
		sKey_LogfileMaxMB         = "Logging.LogfileMaxMB";
		sKey_LogfileMaxFiles      = "Logging.LogfileMaxFiles";

		// Values
		sLogLevel             = "debug";
		bEnableConsoleLogging = false;
		bEnableFileLogging    = true;
		sLogfileFolder        = "./logs";
		iLogfileMaxMB         = 1;
		iLogfileMaxFiles      = 3;
	}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class App_Configuration
{
	// CONSTRUCTOR - DESTRUCTOR		
	public:
		App_Configuration();
		~App_Configuration();

	// public Methods
	public:
		APP_RET_VAL	setConfigFilePath(const string sFilePath);
		APP_RET_VAL	loadConfiguration();
		APP_RET_VAL	dumpConfigToLog();

		APP_RET_VAL getApplicationConf(APP_CONFIG_APPLICATION& outStruct);
		APP_RET_VAL getLoggingConf(APP_CONFIG_LOGGING& outStruct);
		
		APP_RET_VAL	getNumericParam(Int32& outValue, const string sKey);
		APP_RET_VAL getStringParam(string& outValue, const string sKey);

		void setLogger(AutoPtr<Logger> pLogg);

	// private members
		/* Data for handling the configuration */
		bool			  mbIsConfigOK;
		string			  msConfigFilePath;
		JSONConfiguration mJsonConfProvider;

		/*The logger reference we want to use*/
		AutoPtr<Logger>		m_Logger;

};

#endif //APP_CONFIGURATION_H
