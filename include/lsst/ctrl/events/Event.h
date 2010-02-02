// -*- lsst-c++ -*-
/** \file Event.h
  *
  * \ingroup events
  *
  * \brief defines the Event class
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef LSST_CTRL_EVENTS_EVENT_H
#define LSST_CTRL_EVENTS_EVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
/*
#include <cms/TextMessage.h>
#include <cms/MapMessage.h>
*/
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/pex/logging/LogRecord.h"
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

class Event
{
public:
    Event(const std::string& runid, const PropertySet& ps);
    // Event(const std::string& runid, const PropertySet::Ptr& ps);

    ~Event();

    // StringArray getCustomPropertyNames()
    std::string getDate();
    PropertySet::Ptr getPropertySet();
    std::string getPubDate();
    long getPubTime();
    std::string getHostId();
    std::string getRunId();
    std::string getType();
    std::string getStatus();

    std::string getTopic();

    vector<std::string> getCustomPropertyNames();
    void setEventTime(long t);

private:

    // private methods
    void setMessageType(const std::string& msgType);
    long currentTime();
    std::string getIPAddr();

    std::string _type;

    long _eventTime;
    std::string _hostId;
    std::string _runId;
    std::string _status;
    std::string _topic;
    long _pubTime;

    PropertySet::Ptr _psp;

protected:
    void init(const std::string& runId);
};
}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENT_H*/
