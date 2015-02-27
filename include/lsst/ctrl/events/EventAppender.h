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

/** \file EventAppender.h
  *
  * \ingroup events
  *
  * \brief defines the EventAppender class
  *
  */

#ifndef LSST_CTRL_EVENTS_EVENTAPPENDER_H
#define LSST_CTRL_EVENTS_EVENTAPPENDER_H

#include <log4cxx/appenderskeleton.h>
#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/level.h>
#include <limits.h>

#include "lsst/ctrl/events/EventTransmitter.h"

namespace log4cxx {


/**
 * @brief Representation of an LSST Event
 */

class LOG4CXX_EXPORT  EventAppender : public AppenderSkeleton
{
public:


    DECLARE_LOG4CXX_OBJECT(EventAppender)

    BEGIN_LOG4CXX_CAST_MAP()
        LOG4CXX_CAST_ENTRY(EventAppender)
        LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
    END_LOG4CXX_CAST_MAP()

    EventAppender();
    virtual ~EventAppender();

    virtual void setOption(const LogString& option, const LogString& value);

    void append(const spi::LoggingEventPtr& event, log4cxx::helpers::Pool& p);

    void close();

    bool isClosed() const { return closed; }

    bool requiresLayout() const { return false; }

protected:
    lsst::ctrl::events::EventTransmitter *_transmitter;

    int _transmitterInitialized;

    LogString logging_topic;
    LogString broker_host;
    int broker_port;

};
    LOG4CXX_PTR_DEF(EventAppender);
}

#endif /*end LSST_CTRL_EVENTS_EVENTAPPENDER_H*/
