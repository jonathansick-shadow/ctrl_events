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
 * @file EventDequeuer.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Object to receive Events from the specified event queue
 *
 */

#include "lsst/ctrl/events/EventDequeuer.h"
#include "lsst/ctrl/events/Event.h"

namespace lsst {
namespace ctrl {
namespace events {

EventDequeuer::EventDequeuer(const std::string& hostName, const std::string& destinationName, int hostPort) : Receiver() {
    init(hostName, destinationName, "", true, hostPort);
}

EventDequeuer::EventDequeuer(const std::string& hostName, const std::string& destinationName, const std::string& selector, int hostPort) : Receiver() {
    init(hostName, destinationName, selector, true, hostPort);
}

std::string EventDequeuer::getDestinationPropertyName() {
    return Event::QUEUE;
}

std::string EventDequeuer::getQueueName() {
    return getDestinationName();
}

EventDequeuer::~EventDequeuer() {
}

}}}
