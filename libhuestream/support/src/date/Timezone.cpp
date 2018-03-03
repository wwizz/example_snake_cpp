/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "support/date/Timezone.h"

#include <string>

using std::string;

namespace support {

    Timezone::Timezone(const string& name) : _name(string(name)) { }

    Timezone Timezone::utc() {
        return Timezone("UTC");
    }

    Timezone Timezone::local() {
        // Empty timezone is considered as the local timezone
        return Timezone("");
    }

    const string& Timezone::get_name() const {
        return _name;
    }

}  // namespace support
