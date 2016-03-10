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
 * @file Transmitter.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the Transmitter class
 */

#ifndef LSST_CTRL_EVENTS_TRANSMITTER_H
#define LSST_CTRL_EVENTS_TRANSMITTER_H

/*
#include <activemq/commands/ActiveMQTopic.h>
*/
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/daf/base/PropertySet.h"

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/EventBroker.h"

using lsst::daf::base::PropertySet;

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @class Transmitter
 * @brief Transmit events to the event bus
 */

class Transmitter
{
public:
    typedef boost::shared_ptr<Transmitter> Ptr;
    Transmitter();


    /**
     * @brief destructor
     */
    virtual ~Transmitter();

    /**
     * @brief Publish an Event to this object's topic
     * @param event an Event to publish
     */
    void publishEvent(Event& event);

    /**
     * @brief get the destination property name
     */
    virtual std::string getDestinationPropertyName() { return NULL; }

    /**
     * @brief get the destination name of this Transmitter
     * @return a std::string containing the topic name
     */
    std::string getDestinationName();

protected:
    std::string _destinationName;

    // JMS session
    cms::Session* _session;

    void init( const std::string& hostName, const std::string& destinationName, bool createQueue, int port);

private:

    // Connection to JMS broker
    cms::Connection* _connection;

    // Destination to send messages to
    cms::Destination* _destination;

    // Creates messages
    cms::MessageProducer* _producer;

    // internal info about how to contact JMS
    std::string _brokerUri;

};

} } }


#endif /*end LSST_CTRL_EVENTS_TRANSMITTER_H*/
