

#include "App_Includes.h"
#include "App_Definitions.h"
#include "App_Configuration.h"
#include "DDS_SubscriberRunner.h"
#include "DDS_PingPongHandler.h"
#include "App_Main.h"

//############################################################################


//* CONSTRUCTOR **/
App_Main::App_Main() {
   
} // -------------------------------------------------------------------------

//* DESTRUCTOR **/
App_Main::~App_Main() {

} // -------------------------------------------------------------------------

/* Setup the logging system. */
void App_Main::setupLogging() {

    // Get Application and logging configuration
    APP_CONFIG_APPLICATION appConf;
    mpConfiguration->getApplicationConf(appConf);
    APP_CONFIG_LOGGING logConf;
    mpConfiguration->getLoggingConf(logConf);

    // Create logging channels and formatter
    AutoPtr<SplitterChannel>    pSplitter(new SplitterChannel);
    AutoPtr<PatternFormatter>   pPF(new PatternFormatter);
    pPF->setProperty("pattern", APP_LOGGER_FORMAT);

    // Console logger
    if (logConf.bEnableConsoleLogging == true) {
        AutoPtr<ConsoleChannel>     pCons(new ConsoleChannel);
        AutoPtr<FormattingChannel>  pFC(new FormattingChannel(pPF, pCons));
        pSplitter->addChannel(pFC);
    }

    // File Logger
    if (logConf.bEnableFileLogging == true) {
        // create file logging folder when file logging is active
        if (appConf.sName.length() < 1) {
            appConf.sName = this->commandName();
        }
        File logDir(logConf.sLogfileFolder);
        logDir.createDirectories();

        // setup file logging channel
        AutoPtr<FileChannel> pFChannel(new FileChannel);
        string sLogFilePath = format("%s/%s.log", logConf.sLogfileFolder, appConf.sName);
        pFChannel->setProperty("path", sLogFilePath);
        pFChannel->setProperty("rotation", format("%d M", logConf.iLogfileMaxMB));
        pFChannel->setProperty("archive", "timestamp");
        pFChannel->setProperty("times", "utc");
        pFChannel->setProperty("compress", "false");
        pFChannel->setProperty("purgeCount", format("%d", logConf.iLogfileMaxFiles));
        AutoPtr<FormattingChannel> pFCF(new FormattingChannel(pPF, pFChannel));
        AutoPtr<AsyncChannel> pAsync(new AsyncChannel(pFCF));
        pSplitter->addChannel(pAsync);
    }
    Logger::root().setChannel(pSplitter);
    // now get a logger with the name of this app
    m_Logger = &Logger::get(appConf.sName);
    // and finally set the log level
    m_Logger->setLevel(logConf.sLogLevel);

} // -------------------------------------------------------------------------

/* Laod and check configuration files. */
void App_Main::loadConfiguration() {

    // if ConfigFile name is empty (not supplied via command line param), use application name
    if (msConfigFile.length() < 3) {
        msConfigFile = format("%s.json", this->commandName());
    }

    // create configuration class instance
    mpConfiguration.reset(new App_Configuration());

    // Load the data
    mpConfiguration->setConfigFilePath(msConfigFile);
    if (mpConfiguration->loadConfiguration() != AppRetVal_OK) {
        printf("ERROR while loading configuration data - Initalization failed, running with defaults.");
    }
    return;
}

/*Initializes the application and all registered subsystems.*/
void App_Main::initialize(Application& self) {

    // calling base class init
    Application::initialize(self);

    // Load configuration
    this->loadConfiguration();

    // Setup the logging system
    this->setupLogging();

    poco_debug(*m_Logger, "######################################################################################");
    poco_debug(*m_Logger, "# Application is starting UP, initalizing now! Enjoy the show...!");
    poco_debug(*m_Logger, "# All log entry times are UTC.");
    poco_debug(*m_Logger, "######################################################################################");

    // Dump config
    this->mpConfiguration->setLogger(m_Logger);
    this->mpConfiguration->dumpConfigToLog();

    // Create DDS Publisher
    mpDDSParticipant.reset(new DDS_PingPongHandler(m_Logger));

} // -------------------------------------------------------------------------

/*Uninitializes the application and all registered subsystems*/
void App_Main::uninitialize() {

    poco_debug(*m_Logger, "######################################################################################");
    poco_debug(*m_Logger, "# Application is going DOWN. Cleaning up!");
    poco_debug(*m_Logger, "# Have a good day.");
    poco_debug(*m_Logger, "######################################################################################");

    // destroy the DDSPublisher class instance
    mpDDSParticipant.release();

    // destroy the Configuration class instance
    mpConfiguration.release();

    // Stop logger gracefully
    m_Logger->close();
    m_Logger->shutdown();

	// calling base class un-init
    Application::uninitialize();
} // -------------------------------------------------------------------------

/*Re-initializes the application and all registered subsystems*/
void App_Main::reinitialize(Application& self) {

	// calling base class reinit
    Application::reinitialize(self);
} // -------------------------------------------------------------------------
  
/*Called before command line processing begins. If a subclass wants to support command line arguments, it must override this method. */
void App_Main::defineOptions(OptionSet& options) {
    
    Application::defineOptions(options);
   
    options.addOption(Option("help", "h", "display help information on command line arguments")
        .required(false)
        .repeatable(false)
        .noArgument());

    options.addOption(Option("configFile", "c", "specifies a JSON configuration file path to configure this application")
        .required(false)
        .repeatable(false)
        .argument("filePath", true));

}// -------------------------------------------------------------------------

/*Called when the option with the given name is encountered during command line arguments processing*/
void App_Main::handleOption(const std::string& name,
						    const std::string& value) {

    if (name.compare("help") == 0) {
        printf("-----------------> %s\n", this->commandName().c_str());
        printf("Options:\n");
        printf(" /cf <filePath> : Specifies a JSON configuration file path to configure this application\n");
        printf(" /h             : Displays this help\n\n");
        this->stopOptionsProcessing();
    }
    else if (name.compare("configFile") == 0) {
        this->msConfigFile = value;
        printf("Starting up with configuration file = <%s>\n", this->msConfigFile.c_str());
    }

} // -------------------------------------------------------------------------

/*Shows a console menu, waits for user input and then calls action.*/
bool App_Main::consoleMenu() {

    bool   bExit = false;
    bool   bValidInput = false;
    string sInput;

    while (bValidInput == false) {
        std::getline(std::cin, sInput);
        bValidInput = true;

        if (sInput.compare("1") == 0) {
            mpDDSParticipant->startPingListener();
            printf("Now listening to incoming Pings! \n");
        }
        else if (sInput.compare("2") == 0) {
            mpDDSParticipant->stopPingListener();
            printf("Listener stopped! \n");
        }
        else if (sInput.compare("3") == 0) {
            Int32 repeatNr = 1;

            printf("\n Ping: Nr of Pings = ");
            std::getline(std::cin, sInput);
            try {
                repeatNr = NumberParser::parse(sInput, ',');
            }
            catch (...) {
                // nothing
                printf("#   Bad Input!\n");
                break;
            }
            printf("\n Ping: Some Payload Text = ");
            std::getline(std::cin, sInput);

            mpDDSParticipant->sendPing(repeatNr, sInput);
            printf("PING(s) sent! \n");
        }
        else if (sInput.compare("q") == 0) {
            bExit = true;
        }
        else {
            printf("#   Bad Input!\n");
            bValidInput = false;
        }
    }
    
    return bExit;
}

/*The actual main function, starts the Application.*/
int App_Main::main(const std::vector<std::string>& args) {
       
    poco_debug(*m_Logger, "-> main()");

    bool bExit = false;
    printf("#################################################################################\n");
    printf("## DDS Ping Pong - a small app to test DDS connectivity and data exchange       #\n");
    printf("#################################################################################\n");
    printf("#                                                                               #\n");
    printf("# Select an option:                                                             #\n");
    printf("#                                                                               #\n");
    printf("#                                                                               #\n");
    printf("#    1 - Start listening and responding to PING from peers                      #\n");
    printf("#    2 - Stop PING listener                                                     #\n");
    printf("#    3 - Manually publish Ping Message and wait for responses from peers        #\n");
    printf("#                                                                               #\n");
    printf("#    q - Quit this application                                                  #\n");
    printf("# ----------------------------------------------------------------------------- #\n");
    printf("#\n");
    printf("# --> ");

    while (bExit == false) {
        bExit = consoleMenu();
    }
   
    poco_debug(*m_Logger, "<- main()");
    return EXIT_SUCCESS;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// RUN MAIN

POCO_APP_MAIN(App_Main)

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------