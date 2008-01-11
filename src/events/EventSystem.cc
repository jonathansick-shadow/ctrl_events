// -*- lsst-c++ -*-
/** \file EventSystem.cc
  *
  * \brief Coordinate EventTransmitters and EventReceiver objects
  *
  * Author: Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/mwi/data/DataProperty.h"
#include "lsst/mwi/logging/LogRecord.h"
#include "lsst/mwi/policy/Policy.h"
#include "lsst/events/EventLog.h"
#include "lsst/events/EventSystem.h"

using namespace lsst::mwi::data;
using namespace lsst::mwi::logging;

using namespace std;

namespace lsst {
namespace events {
/**
  * \brief EventSystem object.  This object allows creation of the
  *        system's event transmitters and receivers, which can be
  *        specified at the beginning, and later used by specifying
  *        the topic to receive from or send on.
  */
EventSystem::EventSystem() {
}

EventSystem::~EventSystem() {
}
/**      
  * \brief return the default EventSystem object, which can access all 
  *               previously created Transmitters and receivers
  * \return The EventSystem object
  */
const EventSystem& EventSystem::getDefaultEventSystem() {
    if (defaultEventSystem == 0) defaultEventSystem = new EventSystem();
    return *defaultEventSystem;
}
EventSystem *EventSystem::defaultEventSystem = 0;

/**
  * \brief create an EventTransmitter to send messages to the message broker
  * \param hostName the location of the message broker to use
  * \param topicName the topic to transmit events to
  */ 
void EventSystem::createTransmitter(const std::string& hostName, const std::string& topicName) {
    shared_ptr<EventTransmitter> transmitter(new EventTransmitter(hostName, topicName));
    _transmitters.push_back(transmitter);
}

/**
  * \brief create an EventTransmitter to send messages to the message broker
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

/**
  * \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param topicName the topic to receive messages from
  */
void EventSystem::createReceiver(const std::string& hostName, const std::string& topicName) {
    shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, topicName));
    _receivers.push_back(receiver);
}

/**      
  * \brief create an EventReceiver to receive messages from the message broker
  * \param policy the Policy object to use to configure the EventReceiver
  */
void EventSystem::createReceiver(const Policy& policy) {
    shared_ptr<EventReceiver> receiver(new EventReceiver(policy));
    _receivers.push_back(receiver);
}

void EventSystem::createLocalReceiver(const std::string& topicName) {
    Policy policy;
    policy.set("useLocalSockets", true);
    policy.set("topicName", topicName);
    shared_ptr<EventReceiver> receiver(new EventReceiver(policy));
    _receivers.push_back(receiver);
}

void EventSystem::publish(const std::string& topicName, const DataProperty::PtrType dpt) {
    shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
    throw std::runtime_error("topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publish("", dpt);
}

void EventSystem::publish(const std::string& topicName, const LogRecord& rec) {
    shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
    throw std::runtime_error("topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publish(EventLog::getLoggingTopic(), rec);
}


shared_ptr<EventTransmitter> EventSystem::getTransmitter(const std::string& name) {
    list<shared_ptr<EventTransmitter> >::iterator i;
    for (i = _transmitters.begin(); i != _transmitters.end(); i++) {
        if ((*i)->getTopicName() == name)
            return *i;
    }
    return shared_ptr<EventTransmitter>();
}


/**
  * \brief blocking receive for events.  Waits until an event
  *        is received for the topic specified in the constructor
  * \return a DataProperty::PtrType object
  */
DataProperty::PtrType EventSystem::receive(const std::string& topicName) {
    return receive(topicName, EventReceiver::infiniteTimeout);
}

/**
  * \brief blocking receive for events, with timeout (in milliseconds).  
  *        Waits until an event is received for the topic specified
  *        in the constructor, or until the timeout expires.      
  * \return a DataProperty::PtrType object on success, 0 on failure  see note
  *         in receive()
  */
DataProperty::PtrType EventSystem::receive(const std::string& topicName, const long timeout) {
    shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw std::runtime_error("Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->receive(timeout);
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const std::string& value) {
    return matchingReceive(topicName, name, boost::any(value));
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, int value) {
    return matchingReceive(topicName, name, boost::any(value));
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, float value) {
    return matchingReceive(topicName, name, boost::any(value));
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, double value) {
    return matchingReceive(topicName, name, boost::any(value));
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, boost::any value) {
    shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw std::runtime_error("Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->matchingReceive(name, value);
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const std::string& value, long timeout) {
    return matchingReceive(topicName, name, boost::any(value), timeout);
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, int value, long timeout) {
    return matchingReceive(topicName, name, boost::any(value), timeout);
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, float value, long timeout) {
    return matchingReceive(topicName, name, boost::any(value), timeout);
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, double value, long timeout) {
    return matchingReceive(topicName, name, boost::any(value), timeout);
}

DataProperty::PtrType EventSystem::matchingReceive(const std::string& topicName, const std::string& name, boost::any value, long timeout){

    shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw std::runtime_error("Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->matchingReceive(name, value, timeout);
}

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

