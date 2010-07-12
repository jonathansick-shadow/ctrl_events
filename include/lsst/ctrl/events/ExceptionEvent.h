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
 
/** \file ExceptionEvent.h
  *
  * \ingroup events
  *
  * \brief defines the ExceptionEvent class
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef LSST_CTRL_EVENTS_EXCEPTIONEVENT_H
#define LSST_CTRL_EVENTS_EXCEPTIONEVENT_H

#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;

namespace pexPolicy = lsst::pex::policy;
namespace pexLogging = lsst::pex::logging;

namespace lsst {
namespace ctrl {
namespace events { 

/**
 * @brief Representation of an LSST ExceptionEvent
 */

class ExceptionEvent :: public Event
{

public:
    static const std::string EXCEPTIONTYPE;
    static const std::string STACKTRACE;

    ExceptionEvent();
    ExceptionEvent(const std::string& runid, const LSSTException ex);
    ExceptionEvent(cms::TextMessage *msg);

    ~LogEvent();

    void populateHeader(cms::TextMessage *msg);

    std::string getExceptionType();
    std::string getStackTrace();

private:

    void _init();

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_EXCEPTIONEVENT_H*/
