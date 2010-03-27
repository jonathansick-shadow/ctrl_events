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

/** \brief Creates StatusEvent which contains a PropertySet
  *
  */

StatusEvent::StatusEvent() {
    _init();
}

void StatusEvent::_init() {
    _keywords.push_back("ORIGINATOR");
    _keywords.push_back("LOCALID");
    _keywords.push_back("PROCESSID");
}

StatusEvent::StatusEvent(cms::TextMessage *msg, const PropertySet::Ptr psp) : Event(msg, psp) {

    _psp = psp;

    _originator = msg->getLongProperty("ORIGINATOR");
    _eventTime = msg->getShortProperty("LOCALID") ;
    _hostId = msg->getShortProperty("PROCESSID") ;

    _psp->set("ORIGINATOR", _originator);
    _psp->set("LOCALID", _localId);
    _psp->set("PROCESSID", _processId);
}

StatusEvent::StatusEvent( const std::string& runId, const PropertySet::Ptr psp) : Event(runId, psp) {
    _init();
    EventSystem eventSystem = EventSystem().getDefaultEventSystem();
    _originator = eventSystem.createOriginatorId();
    _psp->set("ORIGINATOR",_originator);
    _psp->set("LOCALID",eventSystem.extractLocalId(_originator));
    _psp->set("PROCESSID",eventSystem.extractProcessId(_originator));
}

void StatusEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setStringProperty("ORIGINATOR", _type);
    msg->setStringProperty("LOCALID", _topic);
    msg->setLongProperty("PROCESSID", _eventTime);
}

/** \brief destructor
  */
StatusEvent::~StatusEvent() {
}

}
}
}
