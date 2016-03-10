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

#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLibrary.h"

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
list<EventTransmitter::Ptr> EventSystem::_transmitters;
list<EventReceiver::Ptr> EventSystem::_receivers;
list<EventEnqueuer::Ptr> EventSystem::_enqueuers;
list<EventDequeuer::Ptr> EventSystem::_dequeuers;

void EventSystem::createTransmitter(std::string const& hostName, std::string const& topicName, int hostPort) {
    Transmitter::Ptr transmitter;
    if ((transmitter = getTransmitter(topicName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is already registered with EventSystem");
    EventTransmitter::Ptr evTransmitter(new EventTransmitter(hostName, topicName, hostPort));
    _transmitters.push_back(evTransmitter);
}

void EventSystem::createEnqueuer(std::string const& hostName, std::string const& queueName, int hostPort) {
    Transmitter::Ptr transmitter;
    if ((transmitter = getTransmitter(queueName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "queue "+ queueName + " is already registered with EventSystem");
    EventEnqueuer::Ptr evTransmitter(new EventEnqueuer(hostName, queueName, hostPort));
    _enqueuers.push_back(evTransmitter);
}

void EventSystem::createReceiver(std::string const& hostName, std::string const& topicName, int hostPort) {
    Receiver::Ptr receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
        EventReceiver::Ptr receiver(new EventReceiver(hostName, topicName, hostPort));
        _receivers.push_back(receiver);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is already registered with EventSystem");
}

void EventSystem::createDequeuer(std::string const& hostName, std::string const& queueName, int hostPort) {
    Receiver::Ptr receiver;
    if ((receiver = getReceiver(queueName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "queue "+ queueName + " is already registered with EventSystem");

    EventDequeuer::Ptr evReceiver(new EventDequeuer(hostName, queueName, hostPort));
    _dequeuers.push_back(evReceiver);
}

void EventSystem::createReceiver(std::string const& hostName, std::string const& topicName, std::string const& selector, int hostPort) {
    Receiver::Ptr receiver;
    if ((receiver = getReceiver(topicName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic"+ topicName + " is already registered with EventSystem");

    EventReceiver::Ptr evReceiver(new EventReceiver(hostName, topicName, selector, hostPort));
    _receivers.push_back(evReceiver);
}

void EventSystem::createDequeuer(std::string const& hostName, std::string const& queueName, std::string const& selector, int hostPort) {
    Receiver::Ptr receiver;
    if ((receiver = getReceiver(queueName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "queue"+ queueName + " is already registered with EventSystem");

    EventDequeuer::Ptr evReceiver(new EventDequeuer(hostName, queueName, selector, hostPort));
    _dequeuers.push_back(evReceiver);
}

void EventSystem::publishEvent(std::string const& destinationName, Event& event) {
    Transmitter::Ptr transmitter;
    if ((transmitter = getTransmitter(destinationName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "destination "+ destinationName + " is not registered with EventSystem");
    }
    transmitter->publishEvent(event);
}

/** private method to retrieve a transmitter from the internal list
  */
Transmitter::Ptr EventSystem::getTransmitter(std::string const& name) {
    for (EventTransmitter::Ptr transmitter  : _transmitters) {
        if ((transmitter)->getTopicName() == name) {
            return transmitter;
        }
    }

    for (EventEnqueuer::Ptr qTransmitter : _enqueuers) {
        if ((qTransmitter)->getQueueName() == name) {
            return qTransmitter;
        }
    }
    return Transmitter::Ptr();
}


Event::Ptr EventSystem::receiveEvent(std::string const& destinationName) {
    return receiveEvent(destinationName, EventReceiver::infiniteTimeout);
}

Event::Ptr EventSystem::receiveEvent(std::string const& destinationName, const long timeout) {
    Receiver::Ptr receiver;
    if ((receiver = getReceiver(destinationName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "destination "+ destinationName +" is not registered with EventSystem");
    }

    return receiver->receiveEvent(timeout);
}

LocationId::Ptr EventSystem::createOriginatorId() const {
    return LocationId::Ptr(new LocationId());
}

/** private method used to retrieve the named EventReceiver object
  */
Receiver::Ptr EventSystem::getReceiver(std::string const& name) {
    for (EventReceiver::Ptr receiver : _receivers) {
        if ((receiver)->getTopicName() == name)
            return receiver;
    }
    for (EventDequeuer::Ptr  qReceiver : _dequeuers) {
        if ((qReceiver)->getQueueName() == name)
            return qReceiver;
    }
    return Receiver::Ptr();
}


StatusEvent::Ptr EventSystem::castToStatusEvent(Event::Ptr event) {
    return static_pointer_cast<lsst::ctrl::events::StatusEvent>(event);
}

CommandEvent::Ptr EventSystem::castToCommandEvent(Event::Ptr event) {
    return static_pointer_cast<lsst::ctrl::events::CommandEvent>(event);
}

}}}
