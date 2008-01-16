// -*- lsst-c++ -*-
/** \class EventTransmitter
  *
  * \ingroup events
  *
  * \brief Transmit events to the event bus
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef EVENTTRANSMITTER_H_
#define EVENTTRANSMITTER_H_

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


class EventTransmitter
{
public:
    EventTransmitter(const Policy& policy);

    EventTransmitter(const std::string& hostName, const std::string& topicName);
    ~EventTransmitter();

    void publish(DataProperty::PtrType dpt);
    void publish(DataProperty dp);

    void publish(const std::string& type, DataProperty::PtrType dpt);
    void publish(const std::string& type, DataProperty dp);
    void publish(const std::string& type, const LogRecord& rec);

    std::string getTopicName();

private:
    void init( const std::string& hostName, const std::string& topicName);
    std::string marshall(const DataProperty::PtrType dp, int *cTuples);
    std::string encode(const DataProperty::PtrType dp);
    void publish(const std::string& type, const std::string& messageText, int tuples);

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

    // used to indicate "standalone mode", running without using the 
    // ActiveMQ server
    bool _useLocalSockets;

    // used to completely turn off event  transmission
    bool _turnEventsOff;

    // socket for "standalone mode"
    int _sock;
};
}
}


#endif /*EVENTTRANSMITTER_H_*/
