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
 * @file Transmitter.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Objects to send Events to the specified event bus
 *
 */

#include "lsst/ctrl/events/Transmitter.h"
#include "lsst/ctrl/events/EventLibrary.h"

#include "lsst/daf/base/DateTime.h"
#include "lsst/pex/exceptions.h"

#include <activemq/core/ActiveMQConnectionFactory.h>

namespace dafBase = lsst::daf::base;
namespace pexExceptions = lsst::pex::exceptions;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

Transmitter::Transmitter() {
    EventLibrary().initializeLibrary();
}


/*
 * private initialization method for configuring Transmitter
 */
void Transmitter::init( const std::string& hostName, const std::string& destinationName, bool createQueue, int hostPort) {
    _connection = NULL;
    _session = NULL;

    _producer = NULL;
    _destinationName = destinationName;
    _destination = NULL;

    // set up a connection to the ActiveMQ server for message transmission
    try {
        std::stringstream ss;

        ss << hostPort;

        /*
         * Create a ConnectionFactory to connect to hostName, and
         * create a topic for this.
         */
        string brokerUri = "tcp://"+hostName+":"+ss.str()+"?wireFormat=openwire&transport.useAsyncSend=true";

        activemq::core::ActiveMQConnectionFactory* connectionFactory =
            new activemq::core::ActiveMQConnectionFactory( brokerUri );

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
            _destination = _session->createQueue(_destinationName);
        } else {
            _destination = _session->createTopic(_destinationName);
        }

        // Create a MessageProducer from the Session to the Topic or Queue
        _producer = _session->createProducer(NULL);

        /* XXX - not sure about this setting for queues */
        _producer->setDeliveryMode( cms::DeliveryMode::NON_PERSISTENT );
    } catch ( cms::CMSException& e ) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, std::string("Trouble creating Transmitter: ") + e.getMessage());
    }
}

void Transmitter::publishEvent(Event& event) {
    long long pubtime;
    cms::TextMessage* message = _session->createTextMessage();

    event.marshall(message);

    message->setStringProperty(getDestinationPropertyName(), _destinationName);

    // wait until the last moment to timestamp publication time
    pubtime = dafBase::DateTime::now().nsecs();
    message->setLongProperty("PUBTIME", pubtime);

    _producer->send(_destination, message);
    delete message;
}

std::string Transmitter::getDestinationName() {
    return _destinationName;
}

Transmitter::~Transmitter() {

    if (_destination != NULL)
        delete _destination;
    _destination = NULL;

    try {
        if( _producer != NULL )
            delete _producer;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _producer = NULL;

    // Close open resources.
    try {
        if( _session != NULL )
            _session->close();
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
