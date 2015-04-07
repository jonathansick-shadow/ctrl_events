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
 * @file EventTransmitter.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Objects to send Events to the specified event bus
 *
 */
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cstring>
#include <time.h>

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include <sys/socket.h>
#include <sys/un.h>
#include "lsst/ctrl/events/EventLibrary.h"
#include "lsst/ctrl/events/EventBroker.h"

#include <activemq/core/ActiveMQConnectionFactory.h>

namespace dafBase = lsst::daf::base;
namespace pexExceptions = lsst::pex::exceptions;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

EventTransmitter::EventTransmitter( const std::string& hostName, const std::string& topicName, int hostPort) {
    EventLibrary().initializeLibrary();

    init(hostName, topicName, hostPort);
}

/*
 * private initialization method for configuring EventTransmitter
 */
void EventTransmitter::init( const std::string& hostName, const std::string& topicName, int hostPort) {
    _connection = NULL;
    _session = NULL;
    
    _producer = NULL;
    _topicName = topicName;
    _topic = NULL;

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
       
        // Create the destination topic
        //_destination = _session->createTopic( topicName );

        // Create Topic
        _topic = new activemq::commands::ActiveMQTopic(_topicName);
        

        // Create a MessageProducer from the Session to the Topic or Queue
        _producer = _session->createProducer(NULL);
        _producer->setDeliveryMode( cms::DeliveryMode::NON_PERSISTENT );
    } catch ( cms::CMSException& e ) {
        throw LSST_EXCEPT(pexExceptions::RuntimeError, std::string("Trouble creating EventTransmitter: ") + e.getMessage());
    }
}

void EventTransmitter::publishEvent(Event& event) {
    long long pubtime;
    cms::TextMessage* message = _session->createTextMessage();

    event.marshall(message);

    message->setStringProperty("TOPIC", _topicName);

    // wait until the last moment to timestamp publication time
    pubtime = dafBase::DateTime::now().nsecs();
    message->setLongProperty("PUBTIME", pubtime);

    _producer->send(_topic, message);
    delete message;
}

std::string EventTransmitter::getTopicName() {
    return _topicName;
}

EventTransmitter::~EventTransmitter() {

    if (_topic != NULL)
        delete _topic;
    _topic = NULL;

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

}
}
}
