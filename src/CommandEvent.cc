// -*- lsst-c++ -*-
/** \file CommandEvent.cc
  *
  * \brief Command Event implementation
  *
  * \ingroup ctrl/events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cstring>

#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <time.h>

#include <activemq/core/ActiveMQConnectionFactory.h>


using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

const std::string CommandEvent::ORIGINATORID = "ORIGINATORID";
const std::string CommandEvent::ORIG_LOCALID = "ORIG_LOCALID";
const std::string CommandEvent::ORIG_PROCESSID = "ORIG_PROCESSID";
const std::string CommandEvent::ORIG_IPID = "ORIG_IPID";

const std::string CommandEvent::DESTINATIONID = "DESTINATIONID";
const std::string CommandEvent::DEST_LOCALID = "DEST_LOCALID";
const std::string CommandEvent::DEST_PROCESSID = "DEST_PROCESSID";
const std::string CommandEvent::DEST_IPID = "DEST_IPID";

/** \brief Creates CommandEvent which contains a PropertySet
  *
  */

CommandEvent::CommandEvent() : Event() {
    _init();
}


void CommandEvent::_init() {
    _keywords.push_back(ORIGINATORID);
    _keywords.push_back(ORIG_LOCALID);
    _keywords.push_back(ORIG_PROCESSID);
    _keywords.push_back(ORIG_IPID);

    _keywords.push_back(DESTINATIONID);
    _keywords.push_back(DEST_LOCALID);
    _keywords.push_back(DEST_PROCESSID);
    _keywords.push_back(DEST_IPID);
}

CommandEvent::CommandEvent(cms::TextMessage *msg, const PropertySet::Ptr psp) : Event(msg, psp) {
    _init();

    _psp = psp;

    _originatorId = msg->getLongProperty(ORIGINATORID);
    _orig_localId = msg->getShortProperty(ORIG_LOCALID) ;
    _orig_processId = msg->getShortProperty(ORIG_PROCESSID) ;
    _orig_IPId = msg->getIntProperty(ORIG_IPID) ;

    _destinationId = msg->getLongProperty(DESTINATIONID);
    _dest_localId = msg->getShortProperty(DEST_LOCALID) ;
    _dest_processId = msg->getShortProperty(DEST_PROCESSID) ;
    _dest_IPId = msg->getIntProperty(DEST_IPID) ;
}

void CommandEvent::setKeywords(PropertySet::Ptr psp) const {

    Event::setKeywords(psp);

    psp->set(ORIGINATORID, _originatorId);
    psp->set(ORIG_LOCALID, _orig_localId);
    psp->set(ORIG_PROCESSID, _orig_processId);
    psp->set(ORIG_IPID, _orig_IPId);

    psp->set(DESTINATIONID, _destinationId);
    psp->set(DEST_LOCALID, _dest_localId);
    psp->set(DEST_PROCESSID, _dest_processId);
    psp->set(DEST_IPID, _dest_IPId);
}

CommandEvent::CommandEvent( const std::string& runId, const int64_t originatorId, const int64_t destinationId, const PropertySet::Ptr psp) : Event(runId, psp) {
    _init();


    EventSystem eventSystem = EventSystem().getDefaultEventSystem();

    //_originatorId = eventSystem.createOriginatorId();
    _originatorId = originatorId;
    _orig_localId = eventSystem.extractLocalId(_originatorId);
    _orig_processId = eventSystem.extractProcessId(_originatorId);
    _orig_IPId = eventSystem.extractIPId(_originatorId);

    _destinationId = destinationId;
    _dest_localId = eventSystem.extractLocalId(_destinationId);
    _dest_processId = eventSystem.extractProcessId(_destinationId);
    _dest_IPId = eventSystem.extractIPId(_destinationId);

    _type = EventTypes::COMMAND;

}

void CommandEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setLongProperty(ORIGINATORID, _originatorId);
    msg->setShortProperty(ORIG_LOCALID, _orig_localId);
    msg->setShortProperty(ORIG_PROCESSID, _orig_processId);
    msg->setIntProperty(ORIG_IPID, _orig_IPId);

    msg->setLongProperty(DESTINATIONID, _destinationId);
    msg->setShortProperty(DEST_LOCALID, _dest_localId);
    msg->setShortProperty(DEST_PROCESSID, _dest_processId);
    msg->setIntProperty(DEST_IPID, _dest_IPId);
}

int64_t CommandEvent::getOriginatorId() { return _originatorId; }
short CommandEvent::getOriginatorLocalId() { return _orig_localId; }
short CommandEvent::getOriginatorProcessId() { return _orig_processId; }
int CommandEvent::getOriginatorIPId() { return _orig_IPId; }

int64_t CommandEvent::getDestinationId() { return _destinationId; }
short CommandEvent::getDestinationLocalId() { return _dest_localId; }
short CommandEvent::getDestinationProcessId() { return _dest_processId; }
int CommandEvent::getDestinationIPId() { return _dest_IPId; }

/** \brief destructor
  */
CommandEvent::~CommandEvent() {
}

}
}
}
