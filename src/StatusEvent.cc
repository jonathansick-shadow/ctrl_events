// -*- lsst-c++ -*-
/** \file StatusEvent.cc
  *
  * \brief Status Event implementation
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
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <time.h>

#include <activemq/core/ActiveMQConnectionFactory.h>

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

const std::string StatusEvent::ORIGINATORID = "ORIGINATORID";
const std::string StatusEvent::LOCALID = "LOCALID";
const std::string StatusEvent::PROCESSID = "PROCESSID";
const std::string StatusEvent::IPID = "IPID";

/** \brief Creates StatusEvent which contains a PropertySet
  *
  */

StatusEvent::StatusEvent() : Event() {
    _init();
}


void StatusEvent::_init() {
    _keywords.push_back(ORIGINATORID);
    _keywords.push_back(LOCALID);
    _keywords.push_back(PROCESSID);
    _keywords.push_back(IPID);
}

StatusEvent::StatusEvent(cms::TextMessage *msg, const PropertySet::Ptr psp) : Event(msg, psp) {
    _init();

    _psp = psp;

    _originatorId = msg->getLongProperty(ORIGINATORID);
    _localId = msg->getShortProperty(LOCALID) ;
    _processId = msg->getShortProperty(PROCESSID) ;
    _IPId = msg->getIntProperty(IPID) ;
}

void StatusEvent::setKeywords(PropertySet::Ptr psp) const {

    Event::setKeywords(psp);

    psp->set(ORIGINATORID, _originatorId);
    psp->set(LOCALID, _localId);
    psp->set(PROCESSID, _processId);
    psp->set(IPID, _IPId);
}

StatusEvent::StatusEvent( const std::string& runId, const unsigned long originatorId, const PropertySet::Ptr psp) : Event(runId, *psp) {
    _constructor(runId, originatorId, *psp);
}

StatusEvent::StatusEvent( const std::string& runId, const unsigned long originatorId, const PropertySet& ps) : Event(runId, ps) {
    _constructor(runId, originatorId, ps);
}

void StatusEvent::_constructor(const std::string& runId, const unsigned long originatorId, const PropertySet& ps) {
    _init();


    EventSystem eventSystem = EventSystem().getDefaultEventSystem();

    //_originatorId = eventSystem.createOriginatorId();
    _originatorId = originatorId;

    _localId = eventSystem.extractLocalId(_originatorId);
    _processId = eventSystem.extractProcessId(_originatorId);
    _IPId = eventSystem.extractHostId(_originatorId);
    _type = EventTypes::STATUS;

}

void StatusEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setLongProperty(ORIGINATORID, _originatorId);
    msg->setShortProperty(LOCALID, _localId);
    msg->setShortProperty(PROCESSID, _processId);
    msg->setIntProperty(IPID, _IPId);
}

unsigned long StatusEvent::getOriginatorId() {
    return _originatorId;
}

unsigned short StatusEvent::getLocalId() {
    return _localId;
}

unsigned short StatusEvent::getProcessId() {
    return _processId;
}

unsigned int StatusEvent::getIPId() {
    return _IPId;
}

/** \brief destructor
  */
StatusEvent::~StatusEvent() {
}

}
}
}
