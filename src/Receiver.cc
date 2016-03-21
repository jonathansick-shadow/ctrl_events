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
 * @file Receiver.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Object to receive Events from the specified event queue
 *
 */
#include <iomanip>

#include "lsst/ctrl/events/Receiver.h"

#include "lsst/pex/exceptions.h"

#include "lsst/ctrl/events/EventLibrary.h"
#include "lsst/ctrl/events/EventFactory.h"

#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/exceptions/ActiveMQException.h>

namespace pexExceptions = lsst::pex::exceptions;

namespace activemqCore = activemq::core;

namespace lsst {
namespace ctrl {
namespace events {

Receiver::Receiver() {
    EventLibrary().initializeLibrary();
}

/** private method for initialization of Receiver.
  */
void Receiver::init(const std::string& hostName, const std::string& destinationName, const std::string& selector, bool createQueue, int hostPort) {

    _connection = NULL;
    _session = NULL;
    _destination = NULL;
    _consumer = NULL;
    _destinationName = destinationName;
    _selector = selector;

    try {
        std::stringstream ss;

        ss << hostPort;

        std::string jmsURL = "tcp://"+hostName+":"+ss.str()+"?wireFormat=openwire";

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

        if (createQueue) {
            _destination = _session->createQueue( destinationName );
        } else {
            _destination = _session->createTopic( destinationName );
        }

        if (_selector == "")
            _consumer = _session->createConsumer( _destination );
        else
            _consumer = _session->createConsumer( _destination, selector );

    } catch ( cms::CMSException& e ) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, std::string("Trouble creating Receiver: ") + e.getMessage());
    }
}

PTR(Event) Receiver::receiveEvent() {
    return receiveEvent(infiniteTimeout);
}

PTR(Event) Receiver::receiveEvent(long timeout) {

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

    PTR(Event) event(EventFactory().createEvent(textMessage));
    delete textMessage;

    return event;
}

std::string Receiver::getDestinationName() {
    return _destinationName;
}

std::string Receiver::getSelector() {
    return _selector;
}

Receiver::~Receiver() {

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
