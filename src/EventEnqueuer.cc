// -*- lsst-c++ -*-

/*
 * LSST Data Management System
 * Copyright 2008-2016  AURA/LSST.
 *
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the LSST License Statement and
 * the GNU General Public License along with this program.  If not,
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */

/**
 * @file EventEnqueuer.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Objects to send Events to the specified event bus
 *
 */
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cstring>
#include <time.h>

#include "lsst/ctrl/events/EventEnqueuer.h"

#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include <sys/socket.h>
#include <sys/un.h>

#include "lsst/ctrl/events/EventLibrary.h"

#include <activemq/core/ActiveMQConnectionFactory.h>

namespace dafBase = lsst::daf::base;
namespace pexExceptions = lsst::pex::exceptions;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

EventEnqueuer::EventEnqueuer( const std::string& hostName, const std::string& queueName, int hostPort) : Transmitter() {
    init(hostName, queueName, true, hostPort);
}

std::string EventEnqueuer::getDestinationPropertyName() {
    return "QUEUE";
}

std::string EventEnqueuer::getQueueName() {
    return getDestinationName();
}

EventEnqueuer::~EventEnqueuer() {
}

}}}
