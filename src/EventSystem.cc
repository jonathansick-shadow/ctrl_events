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


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/exceptions.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLibrary.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"

namespace pexLogging =lsst::pex::logging;
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

        // create the _IPId here, rather than
        // reconstructing it every time we create an
        // identificationId

        char buf [255];
        struct hostent *ent;
        unsigned char a,b,c,d;

        gethostname(buf, 255) ;
        ent = (struct hostent *)gethostbyname(buf) ;

        a = ent->h_addr_list[0][0] & 0xFF;
        b = ent->h_addr_list[0][1] & 0xFF;
        c = ent->h_addr_list[0][2] & 0xFF;
        d = ent->h_addr_list[0][3] & 0xFF;

        _IPId = (a << 24) | (b << 16) | (c << 8) | d;
  
        // create the default EventSystem object
        defaultEventSystem = new EventSystem();
    }
    return *defaultEventSystem;

}

EventSystem *EventSystem::defaultEventSystem = 0;
int EventSystem::_IPId = 0;
short EventSystem::_localId = 0;

/** \brief create an EventTransmitter to send messages to the message broker
  * \param hostName the location of the message broker to use
  * \param topicName the topic to transmit events to
  */ 
void EventSystem::createTransmitter(const std::string& hostName, const std::string& topicName) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
        boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(hostName, EventSystem::DEFAULTHOSTPORT, topicName));
        _transmitters.push_back(transmitter);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ topicName + " is already registered with EventSystem");
}

/** \brief create an EventTransmitter to send messages to the message broker
  * \param hostName the location of the message broker to use
  * \param hostPort the port where the broker can be reached
  * \param topicName the topic to transmit events to
  */ 
void EventSystem::createTransmitter(const std::string& hostName, const int hostPort, const std::string& topicName) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
        boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(hostName, hostPort, topicName));
        _transmitters.push_back(transmitter);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ topicName + " is already registered with EventSystem");
}

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
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ transmitter->getTopicName() + " is already registered with EventSystem");
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param topicName the topic to receive messages from
  */
void EventSystem::createReceiver(const std::string& hostName, const std::string& topicName) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
        boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, EventSystem::DEFAULTHOSTPORT, topicName));
        _receivers.push_back(receiver);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ topicName + " is already registered with EventSystem");
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param hostPort the port where the broker can be reached
  * \param topicName the topic to receive messages from
  */
void EventSystem::createReceiver(const std::string& hostName, const int hostPort, const std::string& topicName) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
        boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, hostPort, topicName));
        _receivers.push_back(receiver);
        return;
    }
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ topicName + " is already registered with EventSystem");
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
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic " + receiver->getTopicName() + " is already registered with EventSystem");
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param topicName the topic to receive messages from
  * \param selector the message selector to specify which messages to receive
  */
void EventSystem::createReceiver(const std::string& hostName, const std::string& topicName, const std::string& selector) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, topicName, selector));
    _receivers.push_back(receiver);
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param hostPort the port where the broker can be reached
  * \param topicName the topic to receive messages from
  * \param selector the message selector to specify which messages to receive
  */
void EventSystem::createReceiver(const std::string& hostName, const int hostPort, const std::string& topicName, const std::string& selector) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, hostPort, topicName, selector));
    _receivers.push_back(receiver);
}


/** \brief send an PropertySet on a topic
  * \param topicName the topic to send messages to
  * \param psp the PropertySet to send
  * \throw Runtime exception if the topic wasn't already registered using 
  *        the createTransmitter method
  */
void EventSystem::publish(const std::string& topicName, const PropertySet::Ptr psp) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publish(psp);
}

/** \brief send an logging event
  * \param topicName the topic to send messages to
  * \param rec the LogRecord to send
  * \throw Runtime exception if the topic wasn't already registered using 
  *        the createTransmitter method
  */
void EventSystem::publish(const std::string& topicName, const pexLogging::LogRecord& rec) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publish(rec);
}

/** \brief send an event on a topic
  * \param topicName the topic to send messages to
  * \param event the Event to send
  * \throw Runtime exception if the topic wasn't already registered using 
  *        the createTransmitter method
  */
void EventSystem::publishEvent(const std::string& topicName, const Event& event) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ topicName + " is not registered with EventSystem");
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
PropertySet::Ptr EventSystem::receive(const std::string& topicName) {
    return receive(topicName, EventReceiver::infiniteTimeout);
}

/** \brief blocking receive for events, with timeout (in milliseconds).  
  *        Waits until an event is received for the topic specified
  *        in the constructor, or until the timeout expires.      
  * \param topicName the topic to listen on
  * \param timeout the time in milliseconds to wait before returning
  * \return a Property::Ptr object on success, 0 on failure  see note
  *         in receive()
  */
PropertySet::Ptr EventSystem::receive(const std::string& topicName, const long timeout) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->receive(timeout);
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
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->receiveEvent(timeout);
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

long EventSystem::createOriginatorId() {
    int pid = getpid();
    
   long originatorId = _IPId & 0x0FFFFFFFF;
//
//  switching order of the identifiers to avoid overflow problems.
//
//    originatorId = (originatorId << 32) | (pid << 16) | _localId;
    originatorId = (_localId << 48) | (pid << 32) | originatorId;
    _localId++;
    return originatorId;
}

/** \brief extract the 32-bit hostId embedded in this identificationId.
  *        This is the integer representation of the IPV4 network address
  *        of the host associated with this identificationId
  * \return the 16-bit hostId
  */
int EventSystem::extractIPId(long identificationId) {
    return identificationId & 0xFFFFFFFF;
//    return (identificationId & 0xFFFFFFFF00000000) >> 32;
}

/** \brief extract the 16-bit processId embedded in this identificationId
  * \return the 16-bit processId
  */
short EventSystem::extractProcessId(long identificationId) {
    return (identificationId & 0xFFFF00000000) >> 32;
//    return (identificationId & 0xFFFF0000) >> 16;
}

/** \brief extract the 16-bit localId embedded in this identificationId
  * \return the 16-bit localId
  */
short EventSystem::extractLocalId(long identificationId) {
    return (identificationId & 0xFFFF000000000000) >> 48;
//    return identificationId & 0xFFFF;
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
