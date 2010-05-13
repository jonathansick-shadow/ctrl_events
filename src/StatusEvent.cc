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
    _keywords.insert(ORIGINATORID);
    _keywords.insert(LOCALID);
    _keywords.insert(PROCESSID);
    _keywords.insert(IPID);
}

StatusEvent::StatusEvent(cms::TextMessage *msg) : Event(msg) {
    _init();

    _psp->set(ORIGINATORID, (int64_t)msg->getLongProperty(ORIGINATORID));
    _psp->set(LOCALID, (short)msg->getShortProperty(LOCALID));
    _psp->set(PROCESSID, (short)msg->getShortProperty(PROCESSID));
    _psp->set(IPID, (signed int)msg->getIntProperty(IPID));

}

StatusEvent::StatusEvent( const std::string& runId, const int64_t originatorId, const PropertySet::Ptr psp) : Event(runId, *psp) {
    _constructor(runId, originatorId, *psp);
}

StatusEvent::StatusEvent( const std::string& runId, const int64_t originatorId, const PropertySet& ps) : Event(runId, ps) {
    _constructor(runId, originatorId, ps);
}

void StatusEvent::_constructor(const std::string& runId, const int64_t originatorId, const PropertySet& ps) {
    _init();


    EventSystem eventSystem = EventSystem().getDefaultEventSystem();

    _psp->set(ORIGINATORID, originatorId);

    _psp->set(LOCALID, eventSystem.extractLocalId(originatorId));
    _psp->set(PROCESSID, eventSystem.extractProcessId(originatorId));
    _psp->set(IPID, eventSystem.extractIPId(originatorId));
    _psp->set(TYPE, EventTypes::STATUS);

}

void StatusEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setLongProperty(ORIGINATORID, _psp->get<int64_t>(ORIGINATORID));
    msg->setShortProperty(LOCALID, _psp->get<short>(LOCALID));
    msg->setShortProperty(PROCESSID, _psp->get<short>(PROCESSID));
    msg->setIntProperty(IPID, _psp->get<int>(IPID));
}

int64_t StatusEvent::getOriginatorId() {
    return _psp->get<int64_t>(ORIGINATORID);
}

void StatusEvent::setOriginatorId(int64_t id) {
    return _psp->set(ORIGINATORID, id);
}


short StatusEvent::getLocalId() {
    return _psp->get<short>(LOCALID);
}

short StatusEvent::getProcessId() {
    return _psp->get<short>(PROCESSID);
}

int StatusEvent::getIPId() {
    return _psp->get<int>(IPID);
}

/** \brief destructor
  */
StatusEvent::~StatusEvent() {
}

}
}
}
