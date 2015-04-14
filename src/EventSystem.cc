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
 * @file EventSystem.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Coordinate EventTransmitters and EventReceiver objects
 *
 */
#include <iomanip>
#include <sstream>
#include <stdexcept>


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLibrary.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/Host.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/LocationId.h"

namespace pexExceptions =lsst::pex::exceptions;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

EventSystem::EventSystem() {
    EventLibrary().initializeLibrary();
}

EventSystem::~EventSystem() {
}

EventSystem& EventSystem::getDefaultEventSystem() {
    static EventSystem eventSystem;
    return eventSystem;

}

EventSystem *EventSystem::defaultEventSystem = 0;
list<boost::shared_ptr<EventTransmitter> >EventSystem::_transmitters;
list<boost::shared_ptr<EventReceiver> >EventSystem::_receivers;

void EventSystem::createTransmitter(std::string const& hostName, std::string const& topicName, int hostPort) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
        boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(hostName, topicName, hostPort));
        _transmitters.push_back(transmitter);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is already registered with EventSystem");
}

void EventSystem::createReceiver(std::string const& hostName, std::string const& topicName, int hostPort) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
        boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, topicName, hostPort));
        _receivers.push_back(receiver);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is already registered with EventSystem");
}

void EventSystem::createReceiver(std::string const& hostName, std::string const& topicName, std::string const& selector, int hostPort) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, topicName, selector, hostPort));
    _receivers.push_back(receiver);
}


void EventSystem::publishEvent(std::string const& topicName, Event& event) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publishEvent(event);
}

/** private method to retrieve a transmitter from the internal list
  */
boost::shared_ptr<EventTransmitter> EventSystem::getTransmitter(std::string const& name) {
    list<boost::shared_ptr<EventTransmitter> >::iterator i;
    for (i = _transmitters.begin(); i != _transmitters.end(); i++) {
        if ((*i)->getTopicName() == name) {
            return *i;
        }
    }
    return boost::shared_ptr<EventTransmitter>();
}


Event* EventSystem::receiveEvent(std::string const& topicName) {
    return receiveEvent(topicName, EventReceiver::infiniteTimeout);
}

Event* EventSystem::receiveEvent(std::string const& topicName, const long timeout) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "Topic "+ topicName +" is not registered with EventSystem");
    }

    return receiver->receiveEvent(timeout);
}

LocationId::Ptr EventSystem::createOriginatorId() const {
    return LocationId::Ptr(new LocationId());
}

/** private method used to retrieve the named EventReceiver object
  */
boost::shared_ptr<EventReceiver> EventSystem::getReceiver(std::string const& name) {
    list<boost::shared_ptr<EventReceiver> >::iterator i;
    for (i = _receivers.begin(); i != _receivers.end(); i++) {
        if ((*i)->getTopicName() == name)
            return *i;
    }
    return boost::shared_ptr<EventReceiver>();
}


StatusEvent* EventSystem::castToStatusEvent(Event* event) {
    return (StatusEvent *)event;
}

CommandEvent* EventSystem::castToCommandEvent(Event* event) {
    return (CommandEvent *)event;
}

}}}
