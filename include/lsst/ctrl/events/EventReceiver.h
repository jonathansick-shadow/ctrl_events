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

#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/ctrl/events/Event.h"

using lsst::daf::base::PropertySet;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events { 
/**
 * @class EventReceiver
 * @brief Receive events from the event bus
 */
class EventReceiver {
public:
    /** 
     * @brief Receives events from the specified host and topic
     * @param hostName the machine hosting the message broker
     * @param topicName the topic to receive events from
     * @param hostPort the port the message broker is listening on 
     * \throw throws lsst::pex::exceptions::RuntimeError if connection fails to initialize
     */
    EventReceiver(const std::string& hostName, const std::string& topicName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /** 
     * @brief Receives events from the specified host and topic
     * @param hostName the machine hosting the message broker
     * @param topicName the topic to receive events from
     * @param selector the message selector expression to use.  A selector value of "" is equivalent to no selector.
     * @param hostPort the port the message broker is listening on 
     * \throw throws lsst::pex::exceptions::RuntimeError if connection fails to initialize
     */
    EventReceiver(const std::string& hostName, const std::string& topicName, const std::string& selector, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief destructor
     */
    virtual ~EventReceiver();

    /** 
     * @brief Wait until an Event is received
     * @note Caller is responsible for deleting received Event.
     */
    Event* receiveEvent();

    /**
     * @brief wait for a length of time for an event to be received.
     * @param timeout the length of time to wait in milliseconds; value of -1 waits indefinately.
     * @note Caller is responsible for deleting received Event.
     */
    Event* receiveEvent(long timeout);

    /**
     * @brief get topic name
     * @return the topic name used by this EventReceiver
     */
    std::string getTopicName();

    static const long infiniteTimeout = -1;

private:
    void init(const std::string& hostName, const std::string& topicName, const std::string& selector, int hostPort);

    // connection to the JMS broker
    cms::Connection* _connection;

    // JMS session
    cms::Session* _session;

    // JMS message destination
    cms::Destination* _destination;

    // Object that receives the messages
    cms::MessageConsumer* _consumer;

    // used to completely turn off event  transmission
    bool _turnEventsOff;

    // the topic for this receiver
    std::string _topic;

    // the selector for this receiver
    std::string _selector;

};


}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENTRECEIVER_H*/
