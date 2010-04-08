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

#include <activemq/commands/ActiveMQTopic.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/Event.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/ctrl/events/EventBroker.h"

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

    EventTransmitter(const std::string& hostName, const std::string& topicName, int hostPort = EventBroker::DEFAULTHOSTPORT);
    ~EventTransmitter();

    std::string getTopicName();

    void publishEvent(Event& event);

private:
    void init( const std::string& hostName, const std::string& topicName, int port);

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

    activemq::commands::ActiveMQTopic* _topic;
    std::string _topicName;

    // used to completely turn off event  transmission
    bool _turnEventsOff;

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENTTRANSMITTER_H*/
