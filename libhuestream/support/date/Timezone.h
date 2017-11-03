/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef DATE_TIMEZONE_H_
#define DATE_TIMEZONE_H_

#include <string>

#include "util/StringUtil.h"

namespace huesdk_lib {

    class Timezone {
    public:
        /**
         Construct timezone by name
         @param name The name of the timezone
         */
        explicit Timezone(const std::string& name);
        
        /**
         Get UTC timezone
         @return The UTC timezone
         */
        static Timezone utc();
        
        /**
         Get local timezone, which represents the current
         configured timezone on the machine
         @return The local timezone
         */
        static Timezone local();
        
        /**
         Get name of the timezone
         @return The name of the timezone
         */
        const std::string& get_name() const;
    
    private:
        /** the name of the timezone */
        std::string _name;
    };

}  // namespace huesdk_lib

#endif  // DATE_TIMEZONE_H_
