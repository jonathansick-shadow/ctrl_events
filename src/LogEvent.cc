// -*- lsst-c++ -*-
/** \file LogEvent.cc
  *
  * \brief Object describing the contents of an LogEvent
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

#include <sys/time.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "lsst/ctrl/events/LogEvent.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/LogRecord.h"
#include <sys/socket.h>
#include <sys/un.h>

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

LogEvent::LogEvent(const std::string& runId, const pexLogging::LogRecord& rec) :
Event(runId, rec.getProperties())  {
    _level = 0; // should get it from LogRecord - level
    _log = "uninitialized"; // should get it from LogRecord - log
}

LogEvent::~LogEvent() {
}

std::string LogEvent::getLog() {
    return _log;
}

int LogEvent::getLevel() {
    return _level;
}

}
}
}
