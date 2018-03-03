/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>

#include "support/util/Factory.h"

namespace huesdk {

    using BridgeDiscoveryMethodFactory =
        support::Factory<huesdk::IBridgeDiscoveryMethod, huesdk::BridgeDiscovery::Option>;

    using ScopedBridgeDiscoveryMethodFactory =
        support::ScopedFactory<huesdk::IBridgeDiscoveryMethod, huesdk::BridgeDiscovery::Option>;

}  // namespace huesdk

