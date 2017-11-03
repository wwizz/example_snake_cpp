/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LOGGING_LOGGERFILE_H_
#define LOGGING_LOGGERFILE_H_

#include <mutex>
#include <string>
#include <fstream>  // NOLINT

#include "logging/Logger.h"

using std::mutex;
using std::ofstream;
using std::string;

namespace huesdk_lib {
    namespace log {
        
        class LoggerFile : public Logger {
        public:
            /**
             * Constructor
             */
            LoggerFile(LogLevel level, unsigned int enabled_components, const char* storage_location = "");
            
            /**
             Log the message to the console
             @param msg The message to be logged to the console
             */
            virtual void log(const string& timestamp, LogLevel loglevel, LogComponent component, const string& logtag, const string& msg);
            
        private:
            /** path to the folder where the logfile should be stored */
            std::string _storage_location;
            /** the file stream */
            ofstream _logstream;
        };
        
    }  // namespace log
}  // namespace huesdk_lib

#endif  // LOGGING_LOGGERFILE_H_
