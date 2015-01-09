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
 
/** \file EventSystem.cc
  *
  * \brief Coordinate EventTransmitters and EventReceiver objects
  *
  * \ingroup events
  *
  * \author Stephen R. Pietrowicz, NCSA
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
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/exceptions.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLibrary.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/Host.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/LocationID.h"

namespace pexExceptions =lsst::pex::exceptions;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {
/** \brief EventSystem object.  This object allows creation of the
  *        system's event transmitters and receivers, which can be
  *        specified at the beginning, and later used by specifying
  *        the topic to receive from or send on.
  */
EventSystem::EventSystem() {
    EventLibrary().initializeLibrary();
}

/** \brief destructor
  */
EventSystem::~EventSystem() {
}

/** \brief return the default EventSystem object, which can access all 
  *               previously created Transmitters and receivers
  * \return The EventSystem object
  */
EventSystem& EventSystem::getDefaultEventSystem() {
    if (defaultEventSystem == 0) {

        Host host = Host().getHost();
        _IPId = host.getIPAddress();
        // create the default EventSystem object
        defaultEventSystem = new EventSystem();
    }
    return *defaultEventSystem;

}

EventSystem *EventSystem::defaultEventSystem = 0;
int EventSystem::_IPId = 0;
short EventSystem::_localId = 0;

/** \brief create an EventTransmitter to send messages to the message broker
  * \param policy the Policy object to use to configure the EventTransmitter
  */
void EventSystem::createTransmitter(const pexPolicy::Policy& policy) {
    boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(policy));
    boost::shared_ptr<EventTransmitter> transmitter2;
    if ((transmitter2 = getTransmitter(transmitter->getTopicName())) == 0) {
        _transmitters.push_back(transmitter);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ transmitter->getTopicName() + " is already registered with EventSystem");
}

/** \brief create an EventTransmitter to send messages to the message broker
  * \param hostName the location of the message broker to use
  * \param topicName the topic to transmit events to
  * \param hostPort the port where the broker can be reached
  */ 
void EventSystem::createTransmitter(const std::string& hostName, const std::string& topicName, int hostPort) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
        boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(hostName, topicName, hostPort));
        _transmitters.push_back(transmitter);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is already registered with EventSystem");
}

/** \brief create an EventReceiver to receive messages from the message broker
  * \param policy the Policy object to use to configure the EventReceiver
  */
void EventSystem::createReceiver(const pexPolicy::Policy& policy) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(policy));
    boost::shared_ptr<EventReceiver> receiver2;
    if ((receiver2 = getReceiver(receiver->getTopicName())) == 0) {
        _receivers.push_back(receiver);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic " + receiver->getTopicName() + " is already registered with EventSystem");
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param topicName the topic to receive messages from
  * \param hostPort the port where the broker can be reached
  */
void EventSystem::createReceiver(const std::string& hostName, const std::string& topicName, int hostPort) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
        boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, topicName, hostPort));
        _receivers.push_back(receiver);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is already registered with EventSystem");
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param topicName the topic to receive messages from
  * \param selector the message selector to specify which messages to receive
  * \param hostPort the port where the broker can be reached
  */
void EventSystem::createReceiver(const std::string& hostName, const std::string& topicName, const std::string& selector, int hostPort) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, topicName, selector, hostPort));
    _receivers.push_back(receiver);
}


/** \brief send an event on a topic
  * \param topicName the topic to send messages to
  * \param event the Event to send
  * \throw Runtime exception if the topic wasn't already registered using 
  *        the createTransmitter method
  */
void EventSystem::publishEvent(const std::string& topicName, Event& event) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publishEvent(event);
}

/** private method to retrieve a transmitter from the internal list
  */
boost::shared_ptr<EventTransmitter> EventSystem::getTransmitter(const std::string& name) {
    list<boost::shared_ptr<EventTransmitter> >::iterator i;
    for (i = _transmitters.begin(); i != _transmitters.end(); i++) {
        if ((*i)->getTopicName() == name)
            return *i;
    }
    return boost::shared_ptr<EventTransmitter>();
}


/** \brief blocking receive for events.  Waits until an event
  *        is received for the topic specified in the constructor
  * \param topicName the topic to listen on
  * \return a PropertySet::Ptr object
  */
Event* EventSystem::receiveEvent(const std::string& topicName) {
    return receiveEvent(topicName, EventReceiver::infiniteTimeout);
}

/** \brief blocking receive for events, with timeout (in milliseconds).  
  *        Waits until an event is received for the topic specified
  *        in the constructor, or until the timeout expires.      
  * \param topicName the topic to listen on
  * \param timeout the time in milliseconds to wait before returning
  * \return a Property::Ptr object on success, 0 on failure  see note
  *         in receive()
  */
Event* EventSystem::receiveEvent(const std::string& topicName, const long timeout) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, "Topic "+ topicName +" is not registered with EventSystem");
    }

    return receiver->receiveEvent(timeout);
}

LocationID *EventSystem::createOriginatorID() {
    return new LocationID();
}

/** private method used to retrieve the named EventReceiver object
  */
boost::shared_ptr<EventReceiver> EventSystem::getReceiver(const std::string& name) {
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

}
}
}
