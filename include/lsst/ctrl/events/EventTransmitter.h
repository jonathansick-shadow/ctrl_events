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

namespace dafBase = lsst::daf::base;
namespace pexPolicy = lsst::pex::policy;
namespace pexLogging = lsst::pex::logging;

namespace lsst {
namespace ctrl {
namespace events { 


class EventTransmitter
{
public:
    EventTransmitter(const pexPolicy::Policy& policy);

    EventTransmitter(const std::string& hostName, const std::string& topicName);
    ~EventTransmitter();

    void publish(const dafBase::PropertySet::Ptr& psp);
    void publish(const dafBase::PropertySet& ps);

    void publish(const pexLogging::LogRecord& rec);

    std::string getTopicName();

private:
    void init( const std::string& hostName, const std::string& topicName);
    void publish(const std::string& type, const dafBase::PropertySet& ps);

    std::string marshall(const dafBase::PropertySet& ds);

    // Connection to JMS broker
    cms::Connection* _connection;

    // JMS session
    cms::Session* _session;

    // Destination to send messages to
    cms::Destination* _destination;

    // Creates messages
    cms::MessageProducer* _producer;

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
