/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LOGGING_LOGGERCONSOLE_H_
#define LOGGING_LOGGERCONSOLE_H_

#include <mutex>
#include <string>

#include "logging/Logger.h"

using std::string;
using std::mutex;

namespace huesdk_lib {
    namespace log {
        
        class LoggerConsole : public Logger {
        public:
            /**
             * Constructor
             */
            LoggerConsole(LogLevel level, unsigned int enabled_components);

            /**
             Log the message to the console
             @param msg The message to be logged to the console
             */
            virtual void log(const string& timestamp, LogLevel loglevel, LogComponent component, const string& logtag, const string& msg);
            
        private:
#ifdef ANDROID_LOGCAT
            void android_log(int prio, const char* tag, const char* message);
#endif
        };
    
    }  // namespace log
}  // namespace huesdk_lib

#endif  // LOGGING_LOGGERCONSOLE_H_
