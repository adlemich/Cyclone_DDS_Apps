

#include "App_Includes.h"
#include "App_Definitions.h"
#include "App_Configuration.h"
#include "DDS_SubscriberRunner.h"
#include "DDS_Participant.h"
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
    mpDDSParticipant.reset(new DDS_Participant(m_Logger));

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

    printf("#################################################################################\n");
    printf("## DDS Console Tester - a small app to test DDS connectivity and data exchange  #\n");
    printf("#################################################################################\n");
    printf("#                                                                               #\n");
    printf("# Select an option:                                                             #\n");
    printf("#                                                                               #\n");
    printf("#    phw - Manually publish HelloWorld Message on its topic                     #\n");
    printf("#    shw - Start a subscriber to HelloWorld Messages on its topic               #\n");
    printf("#    thw - Terminate the subscriber to HelloWorld Messages on its topic         #\n");
    printf("#                                                                               #\n");
    printf("#    ppp - Manually publish Ping Message on its topic                           #\n");
    printf("#    spp - Start a subscriber to Ping Messages and respond with Pong            #\n");
    printf("#    tpp - Terminate the subscriber to Ping Messages                            #\n");
    printf("#                                                                               #\n");
    printf("#    q - Quit this application                                                  #\n");
    printf("# ----------------------------------------------------------------------------- #\n");
 
    while (bValidInput == false) {
        printf("# --> ");
        std::getline(std::cin, sInput);
        bValidInput = true;

        if (sInput.compare("spp") == 0) {

            this->mpDDSParticipant->startSubsc_Ping();
            printf("...Startet! Received messages will be logged into the logfile. \n\n\n");
        }
        else if (sInput.compare("tpp") == 0) {

            this->mpDDSParticipant->stopSubsc_Ping();
            printf("...Stopped! \n\n\n");
        }
        else if (sInput.compare("ppp") == 0) {
            printf("\n Ping: pingID = ");
            std::getline(std::cin, sInput);
            Int32 pingId = NumberParser::parse(sInput, ',');
            printf("\n Ping: Some Payload Text = ");
            std::getline(std::cin, sInput);
            mpDDSParticipant->publishMsg_Ping(pingId, sInput);
            printf("...Sent! \n\n\n");
        }
        else if (sInput.compare("thw") == 0) {

            this->mpDDSParticipant->stopSubsc_HelloWorld();
            printf("...Stopped! \n\n\n");
        }
        else if (sInput.compare("shw") == 0) {

            this->mpDDSParticipant->startSubsc_HelloWorld();
            printf("...Startet! Received messages will be logged into the logfile. \n\n\n");
        }
        else if (sInput.compare("phw") == 0) {
            printf("\n HelloWorld: MessageID = ");
            std::getline(std::cin, sInput);
            Int32 msgId = NumberParser::parse(sInput, ',');
            printf("\n HelloWorld: MessageText = ");
            std::getline(std::cin, sInput);
            mpDDSParticipant->publishMsg_HelloWorld(msgId, sInput);
            printf("...Sent! \n\n\n");
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