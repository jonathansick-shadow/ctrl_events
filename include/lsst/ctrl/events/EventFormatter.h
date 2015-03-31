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
 * @file EventFormatter.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the EventFormatter class
 *
 */

#ifndef LSST_CTRL_EVENTS_EVENTFORMATTER_H
#define LSST_CTRL_EVENTS_EVENTFORMATTER_H

#include <stdlib.h>
#include <iostream>

#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/logging/LogFormatter.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"


using namespace std;

namespace pexLogging = lsst::pex::logging;

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @brief class that send a LogRecord into an event stream
 */
class EventFormatter : public pexLogging::LogFormatter {
public:
    explicit EventFormatter() : pexLogging::LogFormatter() { }
    virtual ~EventFormatter();
    void write(ostream *os, const pexLogging::LogRecord& rec);

};

}
}
}
#endif /* end LSST_CTRL_EVENTS_EVENTFORMATTER_H_ */
