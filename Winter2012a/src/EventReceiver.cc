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
 
/** \file EventReceiver.cc
  *
  * \brief Object to receive Events from the specified event bus
  *
  * \ingroup events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cstring>

#include "lsst/ctrl/events/EventFactory.h"
#include "lsst/ctrl/events/EventReceiver.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/LogRecord.h"
#include <sys/socket.h>
#include <sys/un.h>
#include "lsst/ctrl/events/EventLibrary.h"
#include "lsst/ctrl/events/EventBroker.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

namespace pexPolicy = lsst::pex::policy;
namespace pexExceptions = lsst::pex::exceptions;

namespace activemqCore = activemq::core;

namespace lsst {
namespace ctrl {
namespace events {

/** \brief Receives events based on Policy file contents
  *
  * \param policy the policy object to use when building the receiver
  * \throw throws NotFound exception if topicName isn't specified
  * \throw throws NotFound exception if hostName isn't specified
  * \throw throws Runtime exception if connection fails to initialize
  */

EventReceiver::EventReceiver(const pexPolicy::Policy& policy) {
    //EventLibrary().initializeLibrary();
    int hostPort;

    try {
        _turnEventsOff = policy.getBool("turnEventsOff");
    } catch (pexPolicy::NameNotFound& e) {
        _turnEventsOff = false;
    }
    if (_turnEventsOff == true)
        return;

    if (!policy.exists("topicName")) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "topicName not found in policy");
    }

    std::string topicName = policy.getString("topicName");
    try {
        _turnEventsOff = policy.getBool("turnEventsOff");
    } catch (pexPolicy::NameNotFound& e) {
        _turnEventsOff = false;
    }

    if (!policy.exists("hostName")) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "hostName not found in policy");
    }

    std::string hostName = policy.getString("hostName");

    try {
        hostPort = policy.getInt("hostPort");
    } catch (pexPolicy::NameNotFound& e) {
        hostPort = EventBroker::DEFAULTHOSTPORT;
    }

    try {
        _selector = policy.getString("selector");
    } catch (pexPolicy::NameNotFound& e) {
        _selector = "";
    }
    init(hostName, topicName, _selector, hostPort);
}

/** \brief Receives events from the specified host and topic
  *
  * \param hostName the machine hosting the message broker
  * \param topicName the topic to receive events from
  * \param hostPort the port the message broker is listening on 
  * \throw throws Runtime exception if connection fails to initialize
  */
EventReceiver::EventReceiver(const std::string& hostName, const std::string& topicName, int hostPort) {
    _turnEventsOff = false;
    init(hostName, topicName, "", hostPort);
}

/** \brief Receives events from the specified host and topic
  *
  * \param hostName the machine hosting the message broker
  * \param topicName the topic to receive events from
  * \param selector the message selector expression to use.  A selector value of "" is equivalent to no selector.
  * \param hostPort the port the message broker is listening on 
  * \throw throws Runtime exception if connection fails to initialize
  */
EventReceiver::EventReceiver(const std::string& hostName, const std::string& topicName, const std::string& selector, int hostPort) {
    _turnEventsOff = false;
    init(hostName, topicName, selector, hostPort);
}

/** private method for initialization of EventReceiver.  Sets up use of local
  * sockets or activemq, depending on how the policy file was configured.  
  */
void EventReceiver::init(const std::string& hostName, const std::string& topicName, const std::string& selector, int hostPort) {

    EventLibrary().initializeLibrary();
    _connection = NULL;
    _session = NULL;
    _destination = NULL;
    _consumer = NULL;
    _topic = topicName;
    _selector = selector;

    if (_turnEventsOff == true)
        return;

    try {
        std::stringstream ss;

        ss << hostPort;

        string jmsURL = "tcp://"+hostName+":"+ss.str()+"?wireFormat=openwire";

        activemqCore::ActiveMQConnectionFactory* connectionFactory =
            new activemqCore::ActiveMQConnectionFactory( jmsURL );

        _connection = 0;
        try {
            _connection = connectionFactory->createConnection();
            _connection->start();
            delete connectionFactory;
        }
        catch (cms::CMSException& e) {
            delete connectionFactory;
            std::string msg("Failed to connect to broker: ");
            msg += e.getMessage();
            msg += " (is broker running?)";
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, msg);
        }

        _session = _connection->createSession( cms::Session::AUTO_ACKNOWLEDGE );

        _destination = _session->createTopic( topicName );

        if (_selector == "")
            _consumer = _session->createConsumer( _destination );
        else
            _consumer = _session->createConsumer( _destination, selector );

    } catch ( cms::CMSException& e ) {
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, std::string("Trouble creating EventReceiver: ") + e.getMessage());
    }
}

/** Wait until an Event is received
  * Note: Caller is responsible for deleting received Event.
  * \brief wait until an event is received.
  */
Event* EventReceiver::receiveEvent() {
    return receiveEvent(infiniteTimeout);
}

/** Wait to receive an event for a length of time.
  * Note: Caller is responsible for deleting received Event.
  * \brief wait for a length of time for an event to be received.
  * \param timeout the length of time to waitm in milliseconds
  */
Event* EventReceiver::receiveEvent(long timeout) {
    PropertySet::Ptr psp;

    if (_turnEventsOff == true)
        return NULL;

    
    cms::TextMessage* textMessage;
    try {
        cms::Message* msg = _consumer->receive(timeout);
        if (msg == NULL) return NULL;
        textMessage = dynamic_cast<cms::TextMessage* >(msg);
        if (textMessage == NULL)
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "Unexpected JMS Message type");
    } catch (activemq::exceptions::ActiveMQException& e) {
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, e.getMessage());
    }

 
    Event* event = EventFactory().createEvent(textMessage);
    delete textMessage;

    return event;
}

/** \brief returns the topic for this EventReceiver
  */
std::string EventReceiver::getTopicName() {
    return _topic;
}

/** \brief destructor method
  */
EventReceiver::~EventReceiver() {

    // Destroy resources.
    try {
        if( _destination != NULL )
            delete _destination;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _destination = NULL;

    try {
        if( _consumer != NULL )
            delete _consumer;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _consumer = NULL;

    // Close open resources.
    try {
        if( _session != NULL )
            _session->close();
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    try {
        if( _connection != NULL )
            _connection->close();
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }

    try {
        if( _session != NULL )
            delete _session;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _session = NULL;

    try {
        if( _connection != NULL )
            delete _connection;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _connection = NULL;
}

}
}
}
