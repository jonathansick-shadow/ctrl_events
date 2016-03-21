// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2015  AURA/LSST.
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
 * @file EventReceiver.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the EventReceiver class
 *
 */

#ifndef LSST_CTRL_EVENTS_EVENTRECEIVER_H
#define LSST_CTRL_EVENTS_EVENTRECEIVER_H

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
 * @class EventReceiver
 * @brief Receive events from the event bus
 */
class EventReceiver : public Receiver {
public:

    /** 
     * @brief Receives events from the specified host and topic
     * @param hostName the machine hosting the message broker
     * @param destinationName the topic to receive events from
     * @param hostPort the port the message broker is listening on 
     * \throw throws lsst::pex::exceptions::RuntimeError if connection fails to initialize
     */
    EventReceiver(const std::string& hostName, const std::string& destinationName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /** 
     * @brief Receives events from the specified host and topic
     * @param hostName the machine hosting the message broker
     * @param destinationName the topic to receive events from
     * @param selector the message selector expression to use.  A selector value of "" is equivalent to no selector.
     * @param hostPort the port the message broker is listening on 
     * @note The selector allows filtering of messages on the broker before the event is received
     * \throw throws lsst::pex::exceptions::RuntimeError if connection fails to initialize
     */
    EventReceiver(const std::string& hostName, const std::string& destinationName, const std::string& selector, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief destructor
     */
    virtual ~EventReceiver();

    /**
     * @brief get topic name
     * @return the topic name used by this EventReceiver
     */
    std::string getTopicName();

    virtual std::string getDestinationPropertyName();

};


}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENTRECEIVER_H*/
