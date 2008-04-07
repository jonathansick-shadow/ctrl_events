// -*- lsst-c++ -*-
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

#include "lsst/daf/base/DataProperty.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/exceptions.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventSystem.h"

using namespace lsst::daf::base;
using namespace lsst::pex::logging;
using namespace lsst::pex::exceptions;

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
}

/** \brief destructor
  */
EventSystem::~EventSystem() {
}

/** \brief return the default EventSystem object, which can access all 
  *               previously created Transmitters and receivers
  * \return The EventSystem object
  */
const EventSystem& EventSystem::getDefaultEventSystem() {
    if (defaultEventSystem == 0) defaultEventSystem = new EventSystem();
    return *defaultEventSystem;
}
EventSystem *EventSystem::defaultEventSystem = 0;

/** \brief create an EventTransmitter to send messages to the message broker
  * \param hostName the location of the message broker to use
  * \param topicName the topic to transmit events to
  */ 
void EventSystem::createTransmitter(const std::string& hostName, const std::string& topicName) {
    shared_ptr<EventTransmitter> transmitter(new EventTransmitter(hostName, topicName));
    _transmitters.push_back(transmitter);
}

/** \brief create an EventTransmitter to send messages to the message broker
  * \param policy the Policy object to use to configure the EventTransmitter
  */
void EventSystem::createTransmitter(const Policy& policy) {
    shared_ptr<EventTransmitter> transmitter(new EventTransmitter(policy));
    _transmitters.push_back(transmitter);
}

void EventSystem::createLocalTransmitter(const std::string& topicName) {
    Policy policy;
    policy.set("useLocalSockets", true);
    policy.set("topicName", topicName);
    shared_ptr<EventTransmitter> transmitter(new EventTransmitter(policy));
    _transmitters.push_back(transmitter);
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param topicName the topic to receive messages from
  */
void EventSystem::createReceiver(const std::string& hostName, const std::string& topicName) {
    shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, topicName));
    _receivers.push_back(receiver);
}

/** \brief create an EventReceiver to receive messages from the message broker
  * \param policy the Policy object to use to configure the EventReceiver
  */
void EventSystem::createReceiver(const Policy& policy) {
    shared_ptr<EventReceiver> receiver(new EventReceiver(policy));
    _receivers.push_back(receiver);
}

/** \brief create an EventReceiver that uses local sockets to communicate
  * \param topicName the topic to receive messages on
  */
void EventSystem::createLocalReceiver(const std::string& topicName) {
    Policy policy;
    policy.set("useLocalSockets", true);
    policy.set("topicName", topicName);
    shared_ptr<EventReceiver> receiver(new EventReceiver(policy));
    _receivers.push_back(receiver);
}

/** \brief send an event on a topic
  * \param topicName the topic to send messages to
  * \param dpt the DataProperty to send
  * \throw Runtime exception if the topic wasn't already registered using 
  *        the createTransmitter method
  */
void EventSystem::publish(const std::string& topicName, const DataProperty::PtrType dpt) {
    shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
    throw Runtime("topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publish("", dpt);
}

/** \brief send an logging event
  * \param topicName the topic to send messages to
  * \param rec the LogRecord to send
  * \throw Runtime exception if the topic wasn't already registered using 
  *        the createTransmitter method
  */
void EventSystem::publish(const std::string& topicName, const LogRecord& rec) {
    shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
    throw Runtime("topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publish(EventLog::getLoggingTopic(), rec);
}


/** private method to retrieve a transmitter from the internal list
  */
shared_ptr<EventTransmitter> EventSystem::getTransmitter(const std::string& name) {
    list<shared_ptr<EventTransmitter> >::iterator i;
    for (i = _transmitters.begin(); i != _transmitters.end(); i++) {
        if ((*i)->getTopicName() == name)
            return *i;
    }
    return shared_ptr<EventTransmitter>();
}


/** \brief blocking receive for events.  Waits until an event
  *        is received for the topic specified in the constructor
  * \param topicName the topic to listen on
  * \return a DataProperty::PtrType object
  */
DataProperty::PtrType EventSystem::receive(const std::string& topicName) {
    return receive(topicName, EventReceiver::infiniteTimeout);
}

/** \brief blocking receive for events, with timeout (in milliseconds).  
  *        Waits until an event is received for the topic specified
  *        in the constructor, or until the timeout expires.      
  * \param topicName the topic to listen on
  * \param timeout the time in milliseconds to wait before returning
  * \return a DataProperty::PtrType object on success, 0 on failure  see note
  *         in receive()
  */
DataProperty::PtrType EventSystem::receive(const std::string& topicName, const long timeout) {
    shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw Runtime("Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->receive(timeout);
}

/** \brief Receives events matching both the name and string value
  *             
  * \param topicName the topic to receive messages on
  * \param name the DataProperty name to match
  * \param value the string value to match
  * \return the matching DataProperty::PtrType object
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const std::string& value) {
    return matchingReceive(topicName, name, boost::any(value));
}

/** \brief Receives events matching both the name and int value
  *             
  * \param topicName the topic to receive messages on
  * \param name the DataProperty name to match
  * \param value the int value to match
  * \return the matching DataProperty::PtrType object
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, int value) {
    return matchingReceive(topicName, name, boost::any(value));
}

/** \brief Receives events matching both the name and float value
  *             
  * \param topicName the topic to receive messages on
  * \param name the DataProperty name to match
  * \param value the float value to match
  * \return the matching DataProperty::PtrType object
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, float value) {
    return matchingReceive(topicName, name, boost::any(value));
}

/** \brief Receives events matching both the name and double value
  *             
  * \param topicName the topic to receive messages on
  * \param name the DataProperty name to match
  * \param value the double value to match
  * \return the matching DataProperty::PtrType object
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, double value) {
    return matchingReceive(topicName, name, boost::any(value));
}

/** private common method used by all matchingReceive methods
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, boost::any value) {
    shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw Runtime("Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->matchingReceive(name, value);
}

/** \brief Receives events matching both the name and string value within
  *        the specified timeout (in milliseconds). Waits until the
  *        matching event arrives, or until the timeout expires
  *  
  * \param topicName the topic to receive messages on
  * \param name the DataProperty name to match  
  * \param value the string value to match
  * \param timeout the time to wait (in milliseconds)
  * \return the matching DataProperty::PtrType object, 
  *         or a null DataProperty::PtrType object on timeout expiration
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const std::string& value, long timeout) {
    return matchingReceive(topicName, name, boost::any(value), timeout);
}

/** \brief Receives events matching both the name and int value within
  *        the specified timeout (in milliseconds). Waits until the
  *        matching event arrives, or until the timeout expires
  *  
  * \param topicName the topic to receive messages on
  * \param name the DataProperty name to match  
  * \param value the int value to match
  * \param timeout the time to wait (in milliseconds)
  * \return the matching DataProperty::PtrType object, 
  *         or a null DataProperty::PtrType object on timeout expiration
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, int value, long timeout) {
    return matchingReceive(topicName, name, boost::any(value), timeout);
}

/** \brief Receives events matching both the name and float value within
  *        the specified timeout (in milliseconds). Waits until the
  *        matching event arrives, or until the timeout expires
  *  
  * \param topicName the topic to receive messages on
  * \param name the DataProperty name to match  
  * \param value the float value to match
  * \param timeout the time to wait (in milliseconds)
  * \return the matching DataProperty::PtrType object, 
  *         or a null DataProperty::PtrType object on timeout expiration
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, float value, long timeout) {
    return matchingReceive(topicName, name, boost::any(value), timeout);
}

/** \brief Receives events matching both the name and double value within
  *        the specified timeout (in milliseconds). Waits until the
  *        matching event arrives, or until the timeout expires
  *  
  * \param topicName the topic to receive messages on
  * \param name the DataProperty name to match  
  * \param value the double value to match
  * \param timeout the time to wait (in milliseconds)
  * \return the matching DataProperty::PtrType object, 
  *         or a null DataProperty::PtrType object on timeout expiration
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, double value, long timeout) {
    return matchingReceive(topicName, name, boost::any(value), timeout);
}

/** private common method used by all matchingReceive with timeout methods
  */
DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, boost::any value, long timeout){

    shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw Runtime("Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->matchingReceive(name, value, timeout);
}

/** private method used to retrieve the named EventReceiver object
  */
shared_ptr<EventReceiver> EventSystem::getReceiver(const std::string& name) {
    list<shared_ptr<EventReceiver> >::iterator i;
    for (i = _receivers.begin(); i != _receivers.end(); i++) {
        if ((*i)->getTopicName() == name)
            return *i;
    }
    return shared_ptr<EventReceiver>();
}

}
}
}

