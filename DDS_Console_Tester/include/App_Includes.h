#pragma once

//############################################################################
//#### POCO LIBRARY ##########################################################

#include "Poco/Environment.h"
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/PatternFormatter.h"
#include "Poco/Logger.h"
#include "Poco/AutoPtr.h"
#include "Poco/AsyncChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Util/JSONConfiguration.h"
#include "Poco/Exception.h"
#include "Poco/Format.h"
#include "Poco/File.h"
#include "Poco/Util/OptionCallback.h"
#include "Poco/NumberParser.h"
#include "Poco/Thread.h" 
#include "Poco/Runnable.h" 
#include "Poco/ThreadLocal.h"
#include "Poco/Mutex.h"

using namespace Poco;
using namespace Poco::Util;


//############################################################################
//#### DDS library ###########################################################

#include "dds/dds.h"


//############################################################################
//#### standard library ######################################################

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

using namespace std;

//############################################################################
