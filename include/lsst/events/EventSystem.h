// -*- lsst-c++ -*-
/** \file EventSystem.h
  *
  * \ingroup events
  *
  * \brief Coordinate publishing and receiving events
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef LSST_EVENTS_EVENTSYSTEM_H_
#define LSST_EVENTS_EVENTSYSTEM_H_

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#include "lsst/mwi/policy/Policy.h"
#include "lsst/mwi/utils/Component.h"
#include "lsst/mwi/utils/Utils.h"
#include "lsst/mwi/data/DataProperty.h"
#include "lsst/mwi/logging/LogRecord.h"
#include "lsst/events/Events.h"

using lsst::mwi::data::DataProperty;
using lsst::mwi::logging::LogRecord;

using namespace lsst::mwi::data;
using namespace lsst::mwi::policy;
using namespace std;

namespace lsst {
namespace events {
class EventSystem {
public:
    /**
      * \brief EventSystem object.  This object allows creation of the
      *        system's event transmitters and receivers, which can be
      *        specified at the beginning, and later used by specifying
      *        the topic to receive from or send on.
      */
    EventSystem();
    // destructor
    ~EventSystem();

    /**
      * \brief return the default EventSystem object, which can access all previously created Transmitters and receivers
      * \return The EventSystem object
      */
    static const EventSystem& getDefaultEventSystem();

    /**
      * \brief create an EventTransmitter to send messages to the message broker
      * \param hostName the location of the message broker to use
      * \param topicName the topic to transmit events to
      */
    void createTransmitter(const std::string& hostName, const std::string& topicName);
    /**
      * \brief create an EventTransmitter to send messages to the message broker
      * \param policy the Policy object to use to configure the EventTransmitter
      */
    void createTransmitter(const Policy& policy);

    /**
      * \brief create an EventReceiver which will receive message
      * \param hostName the location of the message broker to use
      * \param topicName the topic to receive messages from
      */
    void createReceiver(const std::string& hostName, const std::string& topicName);
    /**
      * \brief create an EventReceiver to receive messages from the message broker
      * \param policy the Policy object to use to configure the EventReceiver
      */
    void createReceiver(const Policy& policy);

    void publish(const std::string& topicName, const DataProperty::PtrType dpt);

    void publish(const std::string& topicName, const LogRecord& rec);

    /**
      * \brief blocking receive for events.  Waits until an event
      *        is received for the topic specified in the constructor
      * \return a DataProperty::PtrType object
      */
    DataProperty::PtrType receive(const std::string& topicName);

    /**
      * \brief blocking receive for events, with timeout (in milliseconds).  
      *        Waits until an event is received for the topic specified
      *        in the constructor, or until the timeout expires.
      * \return a DataProperty::PtrType object on success, 0 on failure  see note
      *         in receive()
      */
    DataProperty::PtrType receive(const std::string& topicName, const long timeout);

    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, const std::string& value);
    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, int value);
    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, float value);
    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, double value);
    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, boost::any value);

    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, const std::string& value, long timeout);
    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, int value, long timeout);
    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, float value, long timeout);
    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, double value, long timeout);
    DataProperty::PtrType matchingReceive(const std::string& topicName, const std::string& name, boost::any value, long timeout);

private:
    shared_ptr<EventTransmitter> getTransmitter(const std::string& name);
    shared_ptr<EventReceiver> getReceiver(const std::string& name);
protected:
    static EventSystem *defaultEventSystem;
    list<shared_ptr<EventTransmitter> >_transmitters;
    list<shared_ptr<EventReceiver> >_receivers;
};
}
}

#endif /*LSST_EVENTS_EVENTSYSTEM_H_*/

