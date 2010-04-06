// -*- lsst-c++ -*-
/** \file MonitorEvent.h
  *
  * \ingroup events
  *
  * \brief defines the Event class
  *
  * \author Stephen Pietrowicz, NCSA
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

namespace pexPolicy = lsst::pex::policy;
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

    ~MonitorEvent();

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
