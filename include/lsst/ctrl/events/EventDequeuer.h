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
 * @file EventDequeuer.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the EventDequeuer class
 *
 */

#ifndef LSST_CTRL_EVENTS_EVENTDEQUEUER_H
#define LSST_CTRL_EVENTS_EVENTDEQUEUER_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

#include "lsst/ctrl/events/Receiver.h"

using lsst::daf::base::PropertySet;

namespace lsst {
namespace ctrl {
namespace events {
/**
 * @class EventDequeuer
 * @brief Receive events from the event bus
 */
class EventDequeuer : public Receiver {
public:
    /**
     * @brief Receives events from the specified host and queue
     * @param hostName the machine hosting the message broker
     * @param destinationName the queue to receive events from
     * @param hostPort the port the message broker is listening on
     * \throw throws lsst::pex::exceptions::RuntimeError if connection fails to initialize
     */
    EventDequeuer(const std::string& hostName, const std::string& destinationName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief Receives events from the specified host and queue
     * @param hostName the machine hosting the message broker
     * @param destinationName the queue to receive events from
     * @param selector the message selector expression to use.  A selector value of "" is equivalent to no selector.
     * @param hostPort the port the message broker is listening on
     * @note The selector allows filtering of messages on the broker before the event is received
     * \throw throws lsst::pex::exceptions::RuntimeError if connection fails to initialize
     */
    EventDequeuer(const std::string& hostName, const std::string& destinationName, const std::string& selector, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief destructor
     */
    virtual ~EventDequeuer();

    /**
     * @brief get queue name
     * @return the queue name used by this EventDequeuer
     */
    std::string getQueueName();

    virtual std::string getDestinationPropertyName();

};


}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENTDEQUEUER_H*/
