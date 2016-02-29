// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2015  AURA/LSST.
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

/** 
 * @file LogEvent.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the LogEvent class
 *
 */

#ifndef LSST_CTRL_EVENTS_LOGEVENT_H
#define LSST_CTRL_EVENTS_LOGEVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/level.h>

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events { 

/**
 * @brief Representation of an LSST Event
 */

class LogEvent : public StatusEvent
{
public:
    static const std::string LEVEL;
    static const std::string LOGGER;


    static const std::string MESSAGE;
    static const std::string TIMESTAMP;
    static const std::string THREADNAME;
    static const std::string FILENAME;
    static const std::string CLASSNAME;
    static const std::string METHODNAME;
    static const std::string LINENUMBER;
    static const std::string LOCATION;

    static const std::string LOGGING_TOPIC;

    LogEvent();
    LogEvent(LocationId const& originatorId, PropertySet const& ps);
    LogEvent(cms::TextMessage *msg);

    virtual ~LogEvent();

    virtual void populateHeader(cms::TextMessage *msg) const;

    int getLevel();

    std::string getLoggingTopic();
    std::string getLogger();

private:
    static const std::string DELIMITER;

    void _init();

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_LOGEVENT_H*/
