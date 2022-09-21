#pragma once

#ifndef APP_DEFINITIONS_H
#define APP_DEFINITIONS_H

/*#############################################################################
*  Method return value definitions
* #############################################################################*/
#define APP_RET_VAL Poco::Int32

constexpr auto AppRetVal_OK			=  1;
constexpr auto AppRetVal_Error		=  0;
constexpr auto AppRetVal_Exception	= -1;
constexpr auto AppRetVal_BadParam   = -2;

/*#############################################################################
*  DEFINITIONS FOR LOGGING
* #############################################################################*/
constexpr auto APP_LOGGER_FORMAT = "%Y-%m-%d %H:%M:%S:%i [%q] %s@%N: %t";




#endif //APP_DEFINITIONS_H
