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
 * @file EventReceiver.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Object to receive Events from the specified event bus
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
#include <sys/socket.h>
#include <sys/un.h>
#include "lsst/ctrl/events/EventLibrary.h"
#include "lsst/ctrl/events/EventBroker.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

namespace pexExceptions = lsst::pex::exceptions;

namespace activemqCore = activemq::core;

namespace lsst {
namespace ctrl {
namespace events {

EventReceiver::EventReceiver(const std::string& hostName, const std::string& topicName, int hostPort) {
    init(hostName, topicName, "", hostPort);
}

EventReceiver::EventReceiver(const std::string& hostName, const std::string& topicName, const std::string& selector, int hostPort) {
    init(hostName, topicName, selector, hostPort);
}

/** private method for initialization of EventReceiver.
  */
void EventReceiver::init(const std::string& hostName, const std::string& topicName, const std::string& selector, int hostPort) {

    EventLibrary().initializeLibrary();
    _connection = NULL;
    _session = NULL;
    _destination = NULL;
    _consumer = NULL;
    _topic = topicName;
    _selector = selector;

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
            throw LSST_EXCEPT(pexExceptions::RuntimeError, msg);
        }

        _session = _connection->createSession( cms::Session::AUTO_ACKNOWLEDGE );

        _destination = _session->createTopic( topicName );

        if (_selector == "")
            _consumer = _session->createConsumer( _destination );
        else
            _consumer = _session->createConsumer( _destination, selector );

    } catch ( cms::CMSException& e ) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, std::string("Trouble creating EventReceiver: ") + e.getMessage());
    }
}

Event* EventReceiver::receiveEvent() {
    return receiveEvent(infiniteTimeout);
}

Event* EventReceiver::receiveEvent(long timeout) {
    PropertySet::Ptr psp;

    cms::TextMessage* textMessage;
    try {
        cms::Message* msg = _consumer->receive(timeout);
        if (msg == NULL) return NULL;
        textMessage = dynamic_cast<cms::TextMessage* >(msg);
        if (textMessage == NULL)
            throw LSST_EXCEPT(pexExceptions::RuntimeError, "Unexpected JMS Message type");
    } catch (activemq::exceptions::ActiveMQException& e) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, e.getMessage());
    }

 
    Event* event = EventFactory().createEvent(textMessage);
    delete textMessage;

    return event;
}

std::string EventReceiver::getTopicName() {
    return _topic;
}

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

}}}
