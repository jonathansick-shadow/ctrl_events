// -*- lsst-c++ -*-
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
    EventLibrary().initializeLibrary();
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

    std::string hostName;
    try {
        hostName = policy.getString("hostName");
    } catch (pexPolicy::NameNotFound& e) {
        hostName = "non";
    }

    try {
        hostPort = policy.getInt("hostPort");
    } catch (pexPolicy::NameNotFound& e) {
        hostPort = EventSystem::DEFAULTHOSTPORT;
    }
    init(hostName, hostPort, topicName);
}

/** \brief Receives events from the specified host and topic
  *
  * \param hostName the machine hosting the message broker
  * \param topicName the topic to receive events from
  * \throw throws Runtime exception if connection fails to initialize
  */
EventReceiver::EventReceiver(const std::string& hostName, const std::string& topicName) {
    EventLibrary().initializeLibrary();

    _turnEventsOff = false;

    init(hostName, EventSystem::DEFAULTHOSTPORT, topicName);
}

/** \brief Receives events from the specified host and topic
  *
  * \param hostName the machine hosting the message broker
  * \param hostPort the port which the message broker is listening on
  * \param topicName the topic to receive events from
  * \throw throws Runtime exception if connection fails to initialize
  */
EventReceiver::EventReceiver(const std::string& hostName, const int hostPort, const std::string& topicName) {
    EventLibrary().initializeLibrary();

    _turnEventsOff = false;

    init(hostName, hostPort, topicName);
}

/** private method for initialization of EventReceiver.  Sets up use of local
  * sockets or activemq, depending on how the policy file was configured.  
  */
void EventReceiver::init(const std::string& hostName, const int hostPort, const std::string& topicName) {

    _connection = NULL;
    _session = NULL;
    _destination = NULL;
    _consumer = NULL;
    _topic = topicName;

    if (_turnEventsOff == true)
        return;

    try {
        std::stringstream ss;

        ss << hostPort;

        string jmsURL = "tcp://"+hostName+":"+ss.str()+"?wireFormat=openwire";

        activemqCore::ActiveMQConnectionFactory* connectionFactory =
            new activemqCore::ActiveMQConnectionFactory( jmsURL );


        _connection = connectionFactory->createConnection();
        delete connectionFactory;
        _connection->start();

        _session = _connection->createSession( cms::Session::AUTO_ACKNOWLEDGE );

        _destination = _session->createTopic( topicName );

        _consumer = _session->createConsumer( _destination );

    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
}

/** \brief blocking receive for events.  Waits until an event
  *        is received for the topic specified in the constructor
  * \return a DataProperty::PtrType object
  * \throw throws Runtime exception if receive fails unexpectedly
  */
PropertySet::Ptr EventReceiver::receive() {
    return _receive(infiniteTimeout);
}

/** \brief blocking receive for events, with timeout (in milliseconds).  
  *        Waits until an event is received or until the timeout expires.
  * \return a DataProperty::PtrType object
  *         in receive()
  * \throw throws Runtime exception if receive fails unexpectedly
  */
PropertySet::Ptr EventReceiver::receive(long timeout) {
    return _receive(timeout);
}

/** private method that performs a regular (non-matching) receive for all
  * variations of the public receive methods.
  */
PropertySet::Ptr EventReceiver::_receive(long timeout) {
    if (_turnEventsOff == true)
        return PropertySet::Ptr();

    try {
            cms::TextMessage* textMessage =
                dynamic_cast<cms::TextMessage* >(_consumer->receive(timeout));
            return processTextMessage(textMessage);
    } catch (activemq::exceptions::ActiveMQException& e) {
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, e.getMessage());
    }
}

Event EventReceiver::receiveEvent() {
    return receiveEvent(infiniteTimeout);
}

Event EventReceiver::receiveEvent(long timeout) {
    PropertySet::Ptr psp;

    if (_turnEventsOff == true)
        return Event();

    std::cout << "0" << std::endl;
    cms::TextMessage* textMessage;
    try {
            textMessage = dynamic_cast<cms::TextMessage* >(_consumer->receive(timeout));
    std::cout << "0a" << std::endl;
            psp =  processTextMessage(textMessage);
    std::cout << "0b" << std::endl;
    } catch (activemq::exceptions::ActiveMQException& e) {
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, e.getMessage());
    }

    std::cout << "1" << std::endl;
    Event event = EventFactory().createEvent(textMessage, psp);
    std::cout << "2" << std::endl;

    return event;
}

/** private method unmarshall the DataProperty from the TextMessage
  */
PropertySet::Ptr EventReceiver::processTextMessage(cms::TextMessage* textMessage) {
    if (textMessage == NULL)
        return PropertySet::Ptr();

    std::string text = textMessage->getText();

    std::cout << "text = " << text << std::endl;
    return unmarshall(text);
}

/** private method unmarshall the DataProperty from a text string
  */
PropertySet::Ptr EventReceiver::unmarshall(const std::string& text) {
    std::vector<string> tuples;

    // split the text into tuples
    splitString(text, "~~", tuples);


    PropertySet::Ptr psp(new PropertySet);

    unsigned int i;
    for (i = 0; i < tuples.size(); i++) {
        std::string line = tuples.at(i);
        std::vector<string> vec;
        splitString(line, "||", vec);

        std::string type = vec.at(0);
        std::string key = vec.at(1);
        std::string val = vec.at(2);

        std::istringstream iss(val);
        boost::any value;

        if (type == "int") {
            int int_value;
            iss >> int_value;
            value = boost::any(int_value);
            psp->add(key, int_value);
        } else if (type == "bool") {
            bool bool_value;
            iss >> bool_value;
            value = boost::any(bool_value);
            psp->add(key, bool_value);
        } else if (type == "long long") {
            long long longlong_value;
            iss >> longlong_value;
            value = boost::any(longlong_value);
            psp->add(key, longlong_value);
        } else if (type == "long") {
            long long_value;
            iss >> long_value;
            value = boost::any(long_value);
            psp->add(key, long_value);
        } else if (type == "float") {
            float float_value;
            iss >> float_value;
            value = boost::any(float_value);
            psp->add(key, float_value);
        } else if (type == "double") {
            double double_value;
            iss >> double_value;
            value = boost::any(double_value);
            psp->add(key, double_value);
        } else if (type == "string") {
            psp->add(key, val);
        } else if (type == "datetime") {
            long long longlong_value;
            iss >> longlong_value;
            psp->add(key, lsst::daf::base::DateTime(longlong_value, lsst::daf::base::DateTime::UTC));
        }
        // type == nodelist can be ignored
    }
        
    return psp;
}

/** private method to split a string along it's delimitors and return the
  * results in a vector
  */
void EventReceiver::splitString(std::string str, std::string delim, 
                                std::vector<std::string>&results) {
    std::string::size_type cutAt;
    std::string::size_type delim_len = delim.length();
    while( (cutAt = str.find(delim)) != str.npos ) {
        if(cutAt > 0) {
            results.push_back(str.substr(0,cutAt));
        }
        str = str.substr(cutAt+delim_len);
    }
    if(str.length() > 0) {
        results.push_back(str);
    }
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
