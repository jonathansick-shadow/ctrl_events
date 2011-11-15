// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
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
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
/** \file EventLog.h
  *
  * \ingroup events
  *
  * \brief defines the EventLog class
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef LSST_CTRL_EVENTS_EVENTLOG_H
#define LSST_CTRL_EVENTS_EVENTLOG_H

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/EventFormatter.h"
#include "lsst/pex/logging/Log.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"


using lsst::daf::base::PropertySet;

namespace pexLogging = lsst::pex::logging;
using namespace std;

namespace lsst {
namespace ctrl {
namespace events {
/**
 * @brief a Log for transmitting log messages as events.
 */
class EventLog : public pexLogging::Log {
public:
    static const std::string LOGGING_TOPIC;

    EventLog( const std::string runId, int sliceId, const std::string hostId = "", int threshold= pexLogging::Log::INFO);
    EventLog( const std::string runId, int sliceId, const PropertySet::Ptr& preamble, const std::string hostId = "", int threshold= pexLogging::Log::INFO);

    virtual ~EventLog();

    int getEventThreshold() { return _log->getThreshold(); }

    void setEventThreshold(int thres) {_log->setThreshold(thres); }


    static void createDefaultLog(const std::string runId, int slideId, const std::string hostId = "", int threshold=Log::INFO); 
    static void createDefaultLog(const std::string runId, int slideId, const PropertySet::Ptr& preamble, const std::string hostId = "", int threshold=Log::INFO); 

private:
    pexLogging::LogDestination *_log;
    EventFormatter *_formatter;
    void init(const std::string runId, int sliceId, const PropertySet::Ptr& preamble, const std::string hostId, int threshold);
    void initThres(int threshold);
};

}
}
}
#endif /* end LSST_CTRL_EVENTS_EVENTLOG_H */
