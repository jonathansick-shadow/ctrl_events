// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2014  AURA/LSST.
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

/** \file EventTransmitter.h
  *
  * \ingroup events
  *
  * \brief defines the EventTransmitter class
  *
  *
  */

#ifndef LSST_CTRL_EVENTS_EVENTTRANSMITTER_H
#define LSST_CTRL_EVENTS_EVENTTRANSMITTER_H

#include <activemq/commands/ActiveMQTopic.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/Event.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/ctrl/events/EventBroker.h"

using lsst::daf::base::PropertySet;

namespace lsst {
namespace ctrl {
namespace events { 

/**
 * @brief Transmit events to the event bus
 */

class EventTransmitter
{
public:
    EventTransmitter(const std::string& hostName, const std::string& topicName, int hostPort = EventBroker::DEFAULTHOSTPORT);
    ~EventTransmitter();

    std::string getTopicName();

    void publishEvent(Event& event);

private:
    void init( const std::string& hostName, const std::string& topicName, int port);

    // Connection to JMS broker
    cms::Connection* _connection;

    // JMS session
    cms::Session* _session;

    // Destination to send messages to
    cms::Destination* _destination;

    // Creates messages
    cms::MessageProducer* _producer;

    // internal info about how to contact JMS
    std::string _brokerUri;

    activemq::commands::ActiveMQTopic* _topic;
    std::string _topicName;

    // used to completely turn off event  transmission
    bool _turnEventsOff;

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENTTRANSMITTER_H*/
