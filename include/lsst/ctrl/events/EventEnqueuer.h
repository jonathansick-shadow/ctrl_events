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
 * @file EventEnqueuer.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the EventEnqueuer class
 */

#ifndef LSST_CTRL_EVENTS_EVENTENQUEUER_H
#define LSST_CTRL_EVENTS_EVENTENQUEUER_H

#include <activemq/commands/ActiveMQTopic.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/Transmitter.h"

using lsst::daf::base::PropertySet;

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @class EventEnqueuer
 * @brief Transmit events to the event bus
 */

class EventEnqueuer : public Transmitter
{
public:
    typedef boost::shared_ptr<EventEnqueuer> Ptr;

    /**
     * @brief Transmits events to the specified host and destination
     *
     * @param hostName the machine hosting the message broker
     * @param destinationName the topic to transmit events to
     * @param hostPort the port number which the message broker is listening to
     * @throws RuntimeError if local socket can't be created
     * @throws RuntimeError if connect to local socket fails
     * @throws RuntimeError if connect to remote ActiveMQ host fails
     */
    EventEnqueuer(const std::string& hostName, const std::string& destinationName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    virtual ~EventEnqueuer();

    /**
     * @brief get the queue name
     * @return a std::string containing the topic name
     */
    std::string getQueueName();

    virtual std::string getDestinationPropertyName();

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENTENQUEUER_H*/
