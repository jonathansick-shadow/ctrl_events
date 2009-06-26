// -*- lsst-c++ -*-
/** \file EventTransmitter.h
  *
  * \ingroup events
  *
  * \brief defines the EventTransmitter class
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef LSST_CTRL_EVENTS_EVENTTRANSMITTER_H
#define LSST_CTRL_EVENTS_EVENTTRANSMITTER_H

#include <cms/Connection.h>
#include <cms/Session.h>
/*
#include <cms/TextMessage.h>
#include <cms/MapMessage.h>
*/
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;

namespace pexPolicy = lsst::pex::policy;
namespace pexLogging = lsst::pex::logging;

namespace lsst {
namespace ctrl {
namespace events { 

/**
 * @brief Transmit events to the event bus
 */

class EventTransmitter
{
public:
    EventTransmitter(const pexPolicy::Policy& policy);

    EventTransmitter(const std::string& hostName, const std::string& topicName);
    EventTransmitter(const std::string& hostName, const std::string& topicName, const PropertySet::Ptr& header);
    ~EventTransmitter();

    void publish(const PropertySet::Ptr& psp);
    void publish(const PropertySet& ps);

    void publish(const pexLogging::LogRecord& rec);

    std::string getTopicName();

    void publish(const std::string& type, const PropertySet& ps);

private:
    // void init( const std::string& hostName, const std::string& topicName);
    void init( const std::string& hostName, const std::string& topicName, const PropertySet::Ptr& header);

    std::string marshall(const PropertySet& ds);

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

protected:
    PropertySet::Ptr _header;
};
}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENTTRANSMITTER_H*/
