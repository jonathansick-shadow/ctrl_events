// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2014  AURA/LSST.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */

/** \file LogEvent.cc
  *
  * \brief Log Event implementation
  *
  * \ingroup ctrl/events
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
    _keywords.insert(LogEvent::COMMENT);
    _keywords.insert(LogEvent::LEVEL);
    _keywords.insert(LogEvent::LOG);
}

LogEvent::LogEvent(cms::TextMessage *msg) : Event(msg) {
    _init();

    vector<std::string>results;

    std::string str = msg->getStringProperty(LogEvent::COMMENT);

    std::string::size_type cutAt;

    std::string::size_type delim_len = LogEvent::DELIMITER.length();
    while( (cutAt = str.find(LogEvent::DELIMITER)) != str.npos ) {
        if(cutAt > 0) {
            results.push_back(str.substr(0,cutAt));
        }
        str = str.substr(cutAt+delim_len);
    }
    if(str.length() > 0) {
        results.push_back(str);
    }

    _psp->set(LogEvent::COMMENT, results);

    _psp->set(LogEvent::LEVEL, msg->getIntProperty(LogEvent::LEVEL));

    _psp->set(LogEvent::LOG, msg->getStringProperty(LogEvent::LOG));

}

LogEvent::LogEvent( const std::string& runId, const pexLogging::LogRecord& rec) : Event(runId, rec.getProperties()) {
    _init();


    const PropertySet& ps = rec.getProperties();

    _psp->set(TYPE, EventTypes::LOG);

    if (!ps.exists(LogEvent::LOG))
        _psp->set(LogEvent::LOG, "default");
    else
        _psp->set(LogEvent::LOG, ps.getAsString(LogEvent::LOG));

    if (!ps.exists(LogEvent::LEVEL))
        _psp->set(LogEvent::LEVEL, -1);
    else
        _psp->set(LogEvent::LEVEL, rec.getImportance());



    if (ps.exists(LogEvent::COMMENT)) {
        std::vector<std::string> commentArray = ps.getArray<std::string>(LogEvent::COMMENT);
    
        _psp->set(LogEvent::COMMENT, commentArray);
    } else {
        _psp->set(LogEvent::COMMENT, "none|@|");
    }
}

void LogEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    std::vector<std::string> vec  = _psp->getArray<std::string>(LogEvent::COMMENT);
    std::ostringstream comment;
    std::vector<std::string>::iterator iter;
    for (iter = vec.begin(); iter != vec.end(); iter++) {
        comment << *iter << LogEvent::DELIMITER;
    }

    msg->setStringProperty(LogEvent::COMMENT, comment.str());

    msg->setIntProperty(LogEvent::LEVEL, _psp->get<int>(LogEvent::LEVEL));

    msg->setStringProperty(LogEvent::LOG, _psp->get<std::string>(LogEvent::LOG));

}

std::vector<std::string> LogEvent::getComment() {
    return _psp->getArray<std::string>(LogEvent::COMMENT);
}

int LogEvent::getLevel() {
    return _psp->get<int>(LogEvent::LEVEL);
}

std::string LogEvent::getLog() {
    return _psp->get<std::string>(LogEvent::LOG);
}

/** \brief destructor
  */
LogEvent::~LogEvent() {
}

}
}
}
