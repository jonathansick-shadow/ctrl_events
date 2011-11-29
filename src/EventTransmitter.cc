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
 
/** \file EventTransmitter.cc
  *
  * \brief Objects to send Events to the specified event bus
  *
  * \ingroup ctrl/events
  *
  * \author Stephen R. Pietrowicz, NCSA
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
#include "lsst/pex/logging/Component.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/LogRecord.h"
#include <sys/socket.h>
#include <sys/un.h>
#include "lsst/ctrl/events/EventLibrary.h"
#include "lsst/ctrl/events/EventBroker.h"

#include <activemq/core/ActiveMQConnectionFactory.h>

namespace dafBase = lsst::daf::base;
namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

/** \brief Configures an EventTransmitter based on Policy contents.
  *
  * The Policy object is checked for four keywords:
  *
  * topicName - the name of the topic to send to       
  * hostName - the name of the host hosting the event broker
  * hostPort - the port the event broker is listening on 
  * turnEventsOff - turn off event transmission
  *
  * Defaults are:
  *
  * turnEventsOff = false
  * hostPort = EventSystem::DEFAULTHOSTPORT
  * 
  * The dependencies for these keywords are as follows:
  *
  * 1) If turnEventsOff is specified as true, no further checking is done, and 
  * no events will be transmitted.
  *
  * 2) If no topicName is specified, a NotFound exception is thrown
  *
  * \param policy the policy object to use when building the receiver
  * \throw throws NotFoundException if expected keywords are missing in Policy object
  * \throw throws RuntimeErrorException if connection to transport mechanism fails
  */
EventTransmitter::EventTransmitter( const pexPolicy::Policy& policy) {
    int hostPort;

    EventLibrary().initializeLibrary();

    try {
        _turnEventsOff = policy.getBool("turnEventsoff");
    } catch (pexPolicy::NameNotFound& e) {
        _turnEventsOff = false;
    }
    if (_turnEventsOff == true)
        return;

    if (!policy.exists("topicName")) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "topicName not found in policy");
    }
    _topicName = policy.getString("topicName");

    std::string hostName;
    try {
        hostName = policy.getString("hostName");
    } catch (pexPolicy::NameNotFound& e) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "hostName not found in policy");
    }

    try {
        hostPort = policy.getInt("hostPort");
    } catch (pexPolicy::NameNotFound& e) {
        hostPort = EventBroker::DEFAULTHOSTPORT;
    }
    init(hostName, _topicName, hostPort);
}

/** \brief Transmits events to the specified host and topic
  *
  * \param hostName the machine hosting the message broker
  * \param topicName the topic to transmit events to
  * \param hostPort the port number which the message broker is listening to
  * \throw throws RuntimeErrorException if local socket can't be created
  * \throw throws RuntimeErrorException if connect to local socket fails
  * \throw throws RuntimeErrorException if connect to remote ActiveMQ host fails
  */
EventTransmitter::EventTransmitter( const std::string& hostName, const std::string& topicName, int hostPort) {
    EventLibrary().initializeLibrary();

    _turnEventsOff = false;
    init(hostName, topicName, hostPort);
}

/** private initialization method for configuring EventTransmitter
  */
void EventTransmitter::init( const std::string& hostName, const std::string& topicName, int hostPort) {
    _connection = NULL;
    _session = NULL;
    // _destination = NULL;
    _producer = NULL;
    _topicName = topicName;
    _topic = NULL;

    if (_turnEventsOff == true)
        return;

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
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, msg);
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
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, std::string("Trouble creating EventTransmitter: ") + e.getMessage());
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

/** \brief get the topic name of this EventTransmitter
  */
std::string EventTransmitter::getTopicName() {
    return _topicName;
}

/** \brief Destructor for EventTransmitter
  */
EventTransmitter::~EventTransmitter() {

/*
    // Destroy resources.
    try {
        if( _destination != NULL )
            delete _destination;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _destination = NULL;
*/
    if (_topic != NULL)
        delete _topic;

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
