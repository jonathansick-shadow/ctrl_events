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
 * @file EventAppender.h
 *
 * @ingroup events
 *
 * @brief defines the EventAppender class
 *
 */

#ifndef LSST_CTRL_EVENTS_EVENTAPPENDER_H
#define LSST_CTRL_EVENTS_EVENTAPPENDER_H

#include <log4cxx/appenderskeleton.h>
#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/helpers/object.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/level.h>
#include <limits.h>

#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/ctrl/events/EventBroker.h"

using namespace log4cxx;
using namespace log4cxx::helpers;

/* this is necessary to get the C++ code to compile correctly in the lsst.ctrl.events namespace  */

#define LSST_IMPLEMENT_LOG4CXX_OBJECT(object)\
const Class& object::getClass() const { return getStaticClass(); }\
const Class& object::getStaticClass() { \
   static Clazz##object theClass;                         \
   return theClass;                                       \
}                                                                      \
const ClassRegistration& object::registerClass() {   \
    static ClassRegistration classReg(object::getStaticClass); \
    return classReg; \
}\
namespace log4cxx { namespace classes { \
const ClassRegistration& object##Registration = object::registerClass(); \
} }

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @class EventAppender
 * @brief Representation of an LSST Event
 */

class EventAppender : public log4cxx::AppenderSkeleton
{
public:

    /* log4cxx macro invocations required in order to expose EventAppender to the log4cxx structure */
    DECLARE_LOG4CXX_OBJECT(EventAppender)

    BEGIN_LOG4CXX_CAST_MAP()
        LOG4CXX_CAST_ENTRY(EventAppender)
        LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
    END_LOG4CXX_CAST_MAP()

    EventAppender();
    ~EventAppender();

    /**
     * @brief log4cxx methods for setting options; for this class options are
     *        BROKER - host name of event broker (required)
     *        PORT - port number on which the event broker is listening (optional)
     *        TOPIC - topic name on which to send logging messages (optional)
     *
     * @param option option name
     * @param option option value
     */
    virtual void setOption(const LogString& option, const LogString& value);

    /* log4cxx methods */
    virtual void activateOptions(log4cxx::helpers::Pool& p);
    void append(const spi::LoggingEventPtr& event, log4cxx::helpers::Pool& p);
    void close();
    bool requiresLayout() const { return false; }

protected:
    lsst::ctrl::events::EventTransmitter* getTransmitter();
    lsst::ctrl::events::EventTransmitter *_transmitter;

    LogString _broker;
    LogString _topic;
    int _port;

};
    LOG4CXX_PTR_DEF(EventAppender);
}
}
}

#endif /*end LSST_CTRL_EVENTS_EVENTAPPENDER_H*/
