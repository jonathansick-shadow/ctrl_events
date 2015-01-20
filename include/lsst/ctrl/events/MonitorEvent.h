// -*- lsst-c++ -*-

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
 
/** \file MonitorEvent.h
  *
  * \ingroup events
  *
  * \brief defines the Event class
  *
  */

#ifndef LSST_CTRL_EVENTS_MONITOREVENT_H
#define LSST_CTRL_EVENTS_MONITOREVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/Event.h"
#include "lsst/pex/policy.h"
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
 * @brief Representation of an LSST Event
 */

class MonitorEvent : public Event
{
public:
    MonitorEvent(const std::string& runid, const pexLogging::LogRecord& rec);

    virtual ~MonitorEvent();

    int getLevel();
    std::string getLog();
    vector<std::string> getComment();

private:

    vector<std::string> _comment;
    int _level;
    std::string _log;

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_MONITORENT_H*/
