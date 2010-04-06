// -*- lsst-c++ -*-
/** \file LogEvent.cc
  *
  * \brief Log Event implementation
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
#include "lsst/ctrl/events/LogEvent.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;


using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

const std::string LogEvent::COMMENT = "COMMENT";
const std::string LogEvent::LEVEL = "LEVEL";
const std::string LogEvent::LOG = "LOG";
const std::string LogEvent::DELIMITER = "|@|";

/** \brief Creates LogEvent which contains a PropertySet
  *
  */

LogEvent::LogEvent() : Event() {
    _init();
}


void LogEvent::_init() {
    _keywords.push_back(COMMENT);
    _keywords.push_back(LEVEL);
    _keywords.push_back(LogEvent::LOG);
}

LogEvent::LogEvent(cms::TextMessage *msg, const PropertySet::Ptr psp) : Event(msg, psp) {
    _init();

    _psp = psp;

    // _comment = msg->getStringProperty(COMMENT);
    _level = msg->getIntProperty(LEVEL);
    _log = msg->getStringProperty(LogEvent::LOG);
}

void LogEvent::setKeywords(PropertySet::Ptr psp) const {

    Event::setKeywords(psp);

    psp->set(COMMENT, _comment);
    psp->set(LEVEL, _level);
    psp->set(LogEvent::LOG, _log);
}

LogEvent::LogEvent( const std::string& runId, const pexLogging::LogRecord& rec) : Event(runId, rec.getProperties()) {
    _init();


    const PropertySet& ps = rec.getProperties();

    _type = EventTypes::LOG;


    _level = rec.getImportance();
    _log = ps.getAsString("LOG");
    _level = ps.getAsInt("LEVEL");

    std::vector<std::string> commentArray = ps.getArray<std::string>("COMMENT");
    
    _comment = commentArray;

}

void LogEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    std::vector<std::string> vec  = _comment;
    std::ostringstream comment;
    std::vector<std::string>::iterator iter;
    for (iter = vec.begin(); iter != vec.end(); iter++) {
        comment << *iter << LogEvent::DELIMITER;
    }
    std::cout << "populate Header comment = " << comment.str() << std::endl;

    msg->setStringProperty(COMMENT, comment.str());
    msg->setIntProperty(LEVEL, _level);
    msg->setStringProperty(LOG, _log);
}

std::vector<std::string> LogEvent::getComment() {
    return _comment;
}

int LogEvent::getLevel() {
    return _level;
}

std::string LogEvent::getLog() {
    return _log;
}

/** \brief destructor
  */
LogEvent::~LogEvent() {
}

}
}
}
