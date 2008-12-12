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
#include <cms/MapMessage.h>
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;
using lsst::pex::logging::LogRecord;

using namespace lsst::daf::base;
using namespace lsst::pex::policy;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::concurrent;
using namespace cms;
using namespace std;

namespace lsst {
namespace ctrl {
namespace events { 


class EventTransmitter
{
public:
    EventTransmitter(const Policy& policy);

    EventTransmitter(const std::string& hostName, const std::string& topicName);
    ~EventTransmitter();

    void publish(const PropertySet::Ptr& psp);
    void publish(const PropertySet& ps);

#ifdef REMOVE_THIS
    void publish(const std::string& type, PropertySet::Ptr psp);
    void publish(const std::string& type, PropertySet ps);
    void publish(const std::string& type, PropertySet ps);
#endif
    void publish(const LogRecord& rec);

    std::string getTopicName();

private:
    void init( const std::string& hostName, const std::string& topicName);
#ifdef REMOVE_THIS
    std::string marshall(const PropertySet::Ptr dp, int *cTuples);
    std::string encode(const PropertySet::Ptr dp);
    void publish(const std::string& type, const std::string& messageText, int tuples);

    void setDate(PropertySet::Ptr dpt);
#endif
    void publish(const std::string& type, const PropertySet& ps);

    std::string marshall(const PropertySet& ds);

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
}


#endif /*EVENTTRANSMITTER_H_*/
