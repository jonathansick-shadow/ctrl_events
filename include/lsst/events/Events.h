// -*- lsst-c++ -*-
/** \file Events.h
  *
  * \ingroup events
  *
  * \brief Transmit and receiver events to the event bus
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef EVENTS_H_
#define EVENTS_H_

#include <activemq/concurrent/Thread.h>
#include <activemq/concurrent/Runnable.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Integer.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/mwi/policy/Policy.h"
#include "lsst/mwi/utils/Component.h"
#include "lsst/mwi/utils/Utils.h"
#include "lsst/mwi/data/DataProperty.h"

using lsst::mwi::data::DataProperty;

using namespace lsst::mwi::data;
using namespace lsst::mwi::policy;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::concurrent;
using namespace cms;
using namespace std;

namespace lsst {
namespace events {
class EventReceiver {
public:
    EventReceiver(Policy* policy);
    /**
      * \brief Receives events from the specified host and topic
      *
      * \param hostName the machine hosting the message broker
      * \param topicName the topic to receive events from
      */
    EventReceiver(const std::string& hostName, const std::string& topicName);

    // virtual destructor
    virtual ~EventReceiver();

    /**
      * \brief blocking receive for events.  Waits until an event
      *        is received for the topic specified in the constructor
      * \return a DataProperty::PtrType object
      */
    DataProperty::PtrType receive();

    /**
      * \brief blocking receive for events, with timeout (in milliseconds).  
      *        Waits until an event is received for the topic specified
      *        in the constructor, or until the timeout expires.
      * \return a DataProperty::PtrType object on success, 0 on failure  see note
      *         in receive()
      */
    DataProperty::PtrType receive(long timeout);

    static const int infiniteTimeout = -1;

private:
    void init(const std::string& hostName, const std::string& topicName);
    // This variables and methods  are used by constructors and methods,  and preclude 
    // specialization.

    DataProperty::PtrType processStandaloneMessage(int sock);
    DataProperty::PtrType unmarshall(int tuples, std::string text);

    // connection to the JMS broker
    Connection* _connection;

    // JMS session
    Session* _session;

    // JMS message destination
    Destination* _destination;

    // Object that receives the messages
    MessageConsumer* _consumer;

    // method to take a TextMessage and convert it to a DataProperty
    DataProperty::PtrType processTextMessage(const TextMessage* textMessage);

    // method to take a string (str) with delimiter (delim) and split it into a vector (results)
    void splitString(std::string str, std::string delim, std::vector<std::string>&results);

    // used to indicate "standalone mode", running without using the ActiveMQ server
    bool _useLocalSockets;

    // used to completely turn off event  transmission
    bool _turnEventsOff;

    // socket for "standalone mode"
    int _sock;
};


class EventTransmitter
{
public:
    EventTransmitter(const Policy& policy);
    /**
      * \brief Transmits events to the specified host and topic
      *
      * \param hostName the machine hosting the message broker
      * \param topicName the topic to transmit events to
      *
      * if the transmitter throws an exception,  calls to
      * send will silently be ignored
      */
    EventTransmitter( const std::string& hostName, const std::string& topicName);
    // destructor
    ~EventTransmitter();

    /**
      * \brief publish an event of "type"  
      *
      * \param type type of event ("log", "exception", some custom name)
      * \param dp The DataProperty to send. Currently only a root 
      *           node with DataProperties below it as children is supported.
      *           Once serialization is added to the DataProperty object, 
      *           arbitrary DataProperty objects will be able to be sent.
      */
    void publish(const std::string& type, DataProperty::PtrType dp);
private:
    void init( const std::string& hostName, const std::string& topicName);
    // This values are used by constructors and methods,  and preclude 
    // specialization.

    // method to serialize a DataProperty into a string;  arg cTuples
    // are the number of tuples added to the serialization
    std::string marshall(const DataProperty::PtrType dp, int *cTuples);

    // Connection to JMS broker
    Connection* _connection;

    // JMS session
    Session* _session;

    // Destination to send messages to
    Destination* _destination;

    // Creates messages
    MessageProducer* _producer;

    // internal info about how to contact JMS
    std::string _brokerUri;

    std::string _topic;

    // used to indicate "standalone mode", running without using the ActiveMQ server
    bool _useLocalSockets;

    // used to completely turn off event  transmission
    bool _turnEventsOff;

    // socket for "standalone mode"
    int _sock;
};
}
}
#endif /*EVENTS_H_*/
