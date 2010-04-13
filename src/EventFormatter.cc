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


using namespace std;
namespace pexLogging = lsst::pex::logging;

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
void EventFormatter::write(ostream *os, const pexLogging::LogRecord& rec) {
    EventSystem system = EventSystem::getDefaultEventSystem();

    const PropertySet& ps = rec.getProperties();
    
    if (!ps.exists(Event::RUNID)) {
        LogEvent event("unspecified",rec);
        system.publishEvent(EventLog::LOGGING_TOPIC, event);
        return;
    } 
    std::string runid = ps.get<std::string>(Event::RUNID);
    LogEvent event(runid,rec);
    system.publishEvent(EventLog::LOGGING_TOPIC, event);
}

}
}
}
