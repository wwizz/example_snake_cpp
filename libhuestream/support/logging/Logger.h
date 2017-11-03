/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LOGGING_LOGGER_H_
#define LOGGING_LOGGER_H_

#include <atomic>
#include <string>

#include "util/StringUtil.h"

using std::string;

namespace huesdk_lib {
    namespace log {

        typedef enum LogLevel {
            LEVEL_OFF       = 0,  // Disable logging
            LEVEL_ERROR     = 1,
            LEVEL_WARN      = 2,
            LEVEL_INFO      = 3,
            LEVEL_DEBUG     = 4,
        } LogLevel;
        
        typedef enum LogComponent {
            COMPONENT_UNKNOWN           = 1,
            COMPONENT_CORE              = 1 << 1,
            COMPONENT_SUPPORT           = 1 << 2,
            COMPONENT_NETWORK           = 1 << 3,
            COMPONENT_WRAPPER           = 1 << 4,
            COMPONENT_CLIENT            = 1 << 5,
            COMPONENT_COLORCONVERSION   = 1 << 6,
            COMPONENT_STREAM            = 1 << 7,
            COMPONENT_STREAM_DTLS       = 1 << 8,
            COMPONENT_TEST              = 1 << 9,
            COMPONENT_ALL               = (1 << 10) - 1,
        } LogComponent;
        
        class Logger {
        public:
            Logger(LogLevel level, unsigned int enabled_components);

            virtual ~Logger();

            bool is_enabled_for(LogLevel level, LogComponent component) const;

            void set_level(LogLevel level);

            LogLevel get_level() const;

            void set_enabled_components(unsigned int enabled_components);

            unsigned int get_enabled_components() const;

            /**
             Log the message
             @param the message to be logged
             */
            virtual void log(const string& timestamp, LogLevel loglevel, LogComponent component, const string& logtag, const string& msg) = 0;
            
            /**
             Translate loglevel to string
             */
            static string get_level_str(LogLevel level);
            
            /**
             Translate logcomponent to string
             */
            static string get_component_str(LogComponent component);
            
            /**
             Get formatted time string, which will be used for
             decorating the message to the logger
             @return The formatted time string
             format: %Y-%m-%d %H:%M:%S.mmm
             format: %Y-%m-%dT%H-%M-%S.mmm (filename-safe)
             */
            static string get_time_str(bool filename_safe = false);
            
        private:
            std::atomic<LogLevel>     _level;
            std::atomic<unsigned int> _enabled_components;
        };
    
    }  // namespace log
}  // namespace huesdk_lib

#endif  // LOGGING_LOGGER_H_
