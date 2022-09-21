#pragma once

#ifndef APP_MAIN_H
#define APP_MAIN_H

class App_Main : public Application
{
	// CONSTRUCTOR - DESTRUCTOR		
	public:
		App_Main();
		~App_Main();

	// protected methods
	protected:
		/*Initializes the application and all registered subsystems.*/
		void initialize(Application& self);
		/*Uninitializes the application and all registered subsystems*/
		void uninitialize();
		/*Re-initializes the application and all registered subsystems*/
		void reinitialize(Application& self);
		/*Called before command line processing begins. If a subclass wants to support command line arguments, it must override this method. */
		void defineOptions(OptionSet& options);
		/*Called when the option with the given name is encountered during command line arguments processing*/
		void handleOption(const string& name,
						  const string& value);	

	// private methods
	private:
		/*Setup the logging system.*/
		void setupLogging();
		/*Load and check configuration files.*/
		void loadConfiguration();
	
	// public methods
	public:
		/*Shows a console menu, waits for user input and then calls action.*/
		bool consoleMenu();
		/*The actual main function, starts the Application.*/
		int main(const vector<string>& args);

	//------------------------------------------------------------------------------
	// MEMBER VARIABLES
	//------------------------------------------------------------------------------
	private:
		/*The logger reference we want to use*/
		AutoPtr<Logger>					m_Logger;
		/*The configuration file name.*/
		string							msConfigFile;
		/*This holds the pointer to the configuration class instance*/
		unique_ptr<App_Configuration>	mpConfiguration;
		/*This holds the pointer to the DDS Participant*/
		unique_ptr<DDS_PingPongHandler>	    mpDDSParticipant;
		
};

#endif /* APP_MAIN_H */

