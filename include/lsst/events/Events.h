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

#include "lsst/mwi/logging/LogRecord.h"
#include "lsst/mwi/policy/Policy.h"
#include "lsst/mwi/utils/Component.h"
#include "lsst/mwi/utils/Utils.h"
#include "lsst/mwi/data/DataProperty.h"

using lsst::mwi::data::DataProperty;
using lsst::mwi::logging::LogRecord;

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
    /**
      * \brief Receives events based on Policy file contents
      *
      * \param policy the policy object to use when building the receiver
      */
    EventReceiver(const Policy& policy);

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
      *        Waits until an event is received or until the timeout expires.
      * \return a DataProperty::PtrType object
      *         in receive()
      */
    DataProperty::PtrType receive(long timeout);

    /**
      * \brief Receives events matching both the name and string value
      *
      * \param name the DataProperty name to match
      * \param value the DataProperty value to match
      * \return the matching DataProperty::PtrType object
      */
    DataProperty::PtrType matchingReceive(const std::string& name, const std::string& value);

    /**
      * \brief Receives events matching both the name and int value
      *
      * \param name the DataProperty name to match
      * \param value the DataProperty value to match
      * \return the matching DataProperty::PtrType object
      */
    DataProperty::PtrType matchingReceive(const std::string& name, int value);

    /**
      * \brief Receives events matching both the name and float value
      *
      * \param name the DataProperty name to match
      * \param value the DataProperty value to match
      * \return the matching DataProperty::PtrType object
      */
    DataProperty::PtrType matchingReceive(const std::string& name, float value);

    /**
      * \brief Receives events matching both the name and double value
      *
      * \param name the DataProperty name to match
      * \param value the DataProperty value to match
      * \return the matching DataProperty::PtrType object
      */
    DataProperty::PtrType matchingReceive(const std::string& name, double value);

    /**
      * \brief Receives events matching both the name and string value within
      *        the specified timeout (in milliseconds). Waits until the
      *        matching event arrives, or until the timeout expires
      *
      * \param name the DataProperty name to match
      * \param value the DataProperty value to match
      * \param timeout the time to wait (in milliseconds)
      * \return the matching DataProperty::PtrType object, 
      *         or a null DataProperty::PtrType object on timeout expiration
      */
    DataProperty::PtrType matchingReceive(const std::string& name, const std::string& value, long timeout);

    /**
      * \brief Receives events matching both the name and string value within
      *        the specified timeout (in milliseconds). Waits until the
      *        matching event arrives, or until the timeout expires
      *
      * \param name the DataProperty name to match
      * \param value the DataProperty value to match
      * \param timeout the time to wait (in milliseconds)
      * \return the matching DataProperty::PtrType object, 
      *         or a null DataProperty::PtrType object on timeout expiration
      */
    DataProperty::PtrType matchingReceive(const std::string& name, int value, long timeout);

    /**
      * \brief Receives events matching both the name and string value within
      *        the specified timeout (in milliseconds). Waits until the
      *        matching event arrives, or until the timeout expires
      *
      * \param name the DataProperty name to match
      * \param value the DataProperty value to match
      * \param timeout the time to wait (in milliseconds)
      * \return the matching DataProperty::PtrType object, 
      *         or a null DataProperty::PtrType object on timeout expiration
      */
    DataProperty::PtrType matchingReceive(const std::string& name, float value, long timeout);

    /**
      * \brief Receives events matching both the name and string value within
      *        the specified timeout (in milliseconds). Waits until the
      *        matching event arrives, or until the timeout expires
      *
      * \param name the DataProperty name to match
      * \param value the DataProperty value to match
      * \param timeout the time to wait (in milliseconds)
      * \return the matching DataProperty::PtrType object, 
      *         or a null DataProperty::PtrType object on timeout expiration
      */
    DataProperty::PtrType matchingReceive(const std::string& name, double value, long timeout);

    // these routines are used by the matchingReceive calls to
    // consolidate the main work into one routine
    DataProperty::PtrType matchingReceive(const std::string& name, boost::any value);
    DataProperty::PtrType matchingReceive(const std::string& name, boost::any value, long timeout);

    /**
      * \brief Get the topic name of this receiver
      * \return a std::string containing the topic name
      */
    std::string getTopicName();

    static const long infiniteTimeout = -1;

private:
    void init(const std::string& hostName, const std::string& topicName);
    // These variables and methods are used by constructors and methods, and 
    // preclude specialization.

    // called by the public receive() and matchingReceive methods
    DataProperty::PtrType _receive();
    // called by the public receive() and matchingReceive with timeout methods
    DataProperty::PtrType _receive(long timeout);

    // unmarshalling routines
    DataProperty::PtrType processStandaloneMessage(int sock);
    DataProperty::PtrType unmarshall(int tuples, std::string text);
    DataProperty::PtrType unmarshall(int tuples, DataProperty::PtrType root, std::vector<string> vec, int& pos);

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

    // the topic for this receiver
    std::string _topic;

    // check for equality of two boost::any values (shouldn't this be in boost?)
    bool equal(boost::any v1, boost::any v2);

    // routine to match values for events that have already been received, but
    // not returned 
    DataProperty::PtrType checkMessageCache(const std::string& name,
                                            boost::any value);


    // this is the simple "cache" used to retain events that weren't matched,
    // but still need to be delivered.
    list<DataProperty::PtrType>_messageCache;

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

    void publish(DataProperty::PtrType dpt);
    void publish(DataProperty dp);

    void publish(const std::string& type, DataProperty::PtrType dpt);
    void publish(const std::string& type, DataProperty dp);
    void publish(const std::string& type, const LogRecord& rec);

    std::string getTopicName();

private:
    void init( const std::string& hostName, const std::string& topicName);
    // This values are used by constructors and methods,  and preclude 
    // specialization.

    // method to serialize a DataProperty into a string;  arg cTuples
    // are the number of tuples added to the serialization
    std::string marshall(const DataProperty::PtrType dp, int *cTuples);
    std::string encode(const DataProperty::PtrType dp);
    void publish(const std::string& type, const std::string& messageText, int tuples);

    // timestamp this event
    void setDate(DataProperty::PtrType dpt);
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
