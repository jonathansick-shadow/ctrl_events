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
 * @file Receiver.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the Receiver class
 *
 */

#ifndef LSST_CTRL_EVENTS_RECEIVER_H
#define LSST_CTRL_EVENTS_RECEIVER_H


#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <stdlib.h>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "lsst/base.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/EventBroker.h"

using lsst::daf::base::PropertySet;

namespace lsst {
namespace ctrl {
namespace events {
/**
 * @class Receiver
 * @brief Receive events from the event bus
 */
class Receiver {
public:
    Receiver();

    /**
     * @brief destructor
     */
    virtual ~Receiver();

    /**
     * @brief Wait until an Event is received
     * @return an Event
     */
    PTR(Event) receiveEvent();

    /**
     * @brief wait for a length of time for an event to be received.
     * @param timeout the length of time to wait in milliseconds; value of -1 waits indefinately.
     * @return an Event
     */
    PTR(Event) receiveEvent(long timeout);

    /**
     * @brief get the destination property name
     * @note This is the TYPE of the destination we're using, either a TOPIC or a QUEUE
     */
    virtual std::string getDestinationPropertyName() { return NULL; }

    /**
     * @brief get destination name
     * @return the destination name used by this Receiver
     */
    std::string getDestinationName();

    /**
     * @brief get selector name
     * @return the selector string used by this receiver
     */
    std::string getSelector();

    static const long infiniteTimeout = -1;

protected:
    std::string _destinationName;
    //
    // JMS session
    cms::Session* _session;

    void init(const std::string& hostName, const std::string& destinationName, const std::string& selector, bool createQueue, int hostPort);

private:

    // connection to the JMS broker
    cms::Connection* _connection;

    // JMS message destination
    cms::Destination* _destination;

    // Object that receives the messages
    cms::MessageConsumer* _consumer;

    // the selector for this receiver
    std::string _selector;

};


}
}
}


#endif /*end LSST_CTRL_EVENTS_RECEIVER_H*/
