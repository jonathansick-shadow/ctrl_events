// -*- lsst-c++ -*-
/** \file EventFormatter.cc
  *
  * \brief EventFormatter class required for logging support for events
  *
  * \ingroup events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFormatter.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/logging/Component.h"

//using namespace lsst::daf::base;
using namespace lsst::pex::logging;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

EventFormatter::~EventFormatter() {}


/** \brief writes a record to the event log stream.   This ignores the
  *        ostream, but this is required because of the signature of this
  *        required method
  * \param os output stream.  this is ignored, since the output stream
  *           is the event stream; it's a placeholder because of the
  *           signature of this method.
  * \param rec the LogRecord to send to the event topic;
  */
void EventFormatter::write(ostream *os, const LogRecord& rec) {
    EventSystem system = EventSystem::getDefaultEventSystem();
    system.publish(EventLog::getLoggingTopic(), rec);
}

}
}
}
