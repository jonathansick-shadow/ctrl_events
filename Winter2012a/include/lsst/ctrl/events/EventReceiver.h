// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
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
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
/** \file EventReceiver.h
  *
  * \ingroup events
  *
  * \brief defines the EventReceiver class
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef LSST_CTRL_EVENTS_EVENTRECEIVER_H
#define LSST_CTRL_EVENTS_EVENTRECEIVER_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/pex/policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/ctrl/events/Event.h"

using lsst::daf::base::PropertySet;

namespace pexPolicy = lsst::pex::policy;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events { 
/**
 * @brief Receive events from the event bus
 */
class EventReceiver {
public:
    EventReceiver(const pexPolicy::Policy& policy);

    EventReceiver(const std::string& hostName, const std::string& topicName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    EventReceiver(const std::string& hostName, const std::string& topicName, const std::string& selector, int hostPort = EventBroker::DEFAULTHOSTPORT);

    // virtual destructor
    virtual ~EventReceiver();

    Event* receiveEvent();
    Event* receiveEvent(long timeout);

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
