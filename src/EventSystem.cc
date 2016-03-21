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

#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLibrary.h"

namespace pexExceptions = lsst::pex::exceptions;

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
std::list<PTR(EventTransmitter)> EventSystem::_transmitters;
std::list<PTR(EventReceiver)> EventSystem::_receivers;
std::list<PTR(EventEnqueuer)> EventSystem::_enqueuers;
std::list<PTR(EventDequeuer)> EventSystem::_dequeuers;

void EventSystem::createTransmitter(std::string const& hostName, std::string const& topicName, int hostPort) {
    PTR(Transmitter) transmitter;
    if ((transmitter = getTransmitter(topicName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is already registered with EventSystem");
    PTR(EventTransmitter) evTransmitter(new EventTransmitter(hostName, topicName, hostPort));
    _transmitters.push_back(evTransmitter);
}

void EventSystem::createEnqueuer(std::string const& hostName, std::string const& queueName, int hostPort) {
    PTR(Transmitter) transmitter;
    if ((transmitter = getTransmitter(queueName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "queue "+ queueName + " is already registered with EventSystem");
    PTR(EventEnqueuer) evTransmitter(new EventEnqueuer(hostName, queueName, hostPort));
    _enqueuers.push_back(evTransmitter);
}

void EventSystem::createReceiver(std::string const& hostName, std::string const& topicName, int hostPort) {
    PTR(Receiver) receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
        PTR(EventReceiver) receiver(new EventReceiver(hostName, topicName, hostPort));
        _receivers.push_back(receiver);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is already registered with EventSystem");
}

void EventSystem::createDequeuer(std::string const& hostName, std::string const& queueName, int hostPort) {
    PTR(Receiver) receiver;
    if ((receiver = getReceiver(queueName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "queue "+ queueName + " is already registered with EventSystem");

    PTR(EventDequeuer) evReceiver(new EventDequeuer(hostName, queueName, hostPort));
    _dequeuers.push_back(evReceiver);
}

void EventSystem::createReceiver(std::string const& hostName, std::string const& topicName, std::string const& selector, int hostPort) {
    PTR(Receiver) receiver;
    if ((receiver = getReceiver(topicName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic"+ topicName + " is already registered with EventSystem");

    PTR(EventReceiver) evReceiver(new EventReceiver(hostName, topicName, selector, hostPort));
    _receivers.push_back(evReceiver);
}

void EventSystem::createDequeuer(std::string const& hostName, std::string const& queueName, std::string const& selector, int hostPort) {
    PTR(Receiver) receiver;
    if ((receiver = getReceiver(queueName)) != 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "queue"+ queueName + " is already registered with EventSystem");

    PTR(EventDequeuer) evReceiver(new EventDequeuer(hostName, queueName, selector, hostPort));
    _dequeuers.push_back(evReceiver);
}

void EventSystem::publishEvent(std::string const& destinationName, Event& event) {
    PTR(Transmitter) transmitter;
    if ((transmitter = getTransmitter(destinationName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "destination "+ destinationName + " is not registered with EventSystem");
    }
    transmitter->publishEvent(event);
}

/** private method to retrieve a transmitter from the internal list
  */
PTR(Transmitter) EventSystem::getTransmitter(std::string const& name) {
    for (PTR(EventTransmitter) transmitter  : _transmitters) {
        if ((transmitter)->getTopicName() == name) {
            return transmitter;
        }
    }

    for (PTR(EventEnqueuer) qTransmitter : _enqueuers) {
        if ((qTransmitter)->getQueueName() == name) {
            return qTransmitter;
        }
    }
    return PTR(Transmitter)();
}


PTR(Event) EventSystem::receiveEvent(std::string const& destinationName) {
    return receiveEvent(destinationName, EventReceiver::infiniteTimeout);
}

PTR(Event) EventSystem::receiveEvent(std::string const& destinationName, const long timeout) {
    PTR(Receiver) receiver;
    if ((receiver = getReceiver(destinationName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "destination "+ destinationName +" is not registered with EventSystem");
    }

    return receiver->receiveEvent(timeout);
}

PTR(LocationId) EventSystem::createOriginatorId() const {
    return PTR(LocationId)(new LocationId());
}

/** private method used to retrieve the named EventReceiver object
  */
PTR(Receiver) EventSystem::getReceiver(std::string const& name) {
    for (PTR(EventReceiver) receiver : _receivers) {
        if ((receiver)->getTopicName() == name)
            return receiver;
    }
    for (PTR(EventDequeuer)  qReceiver : _dequeuers) {
        if ((qReceiver)->getQueueName() == name)
            return qReceiver;
    }
    return PTR(Receiver)();
}


PTR(StatusEvent) EventSystem::castToStatusEvent(PTR(Event) event) {
    return boost::static_pointer_cast<lsst::ctrl::events::StatusEvent>(event);
}

PTR(CommandEvent) EventSystem::castToCommandEvent(PTR(Event) event) {
    return boost::static_pointer_cast<lsst::ctrl::events::CommandEvent>(event);
}

}}}
