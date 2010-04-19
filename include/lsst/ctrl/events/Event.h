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
#include <cms/Message.h>

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

    static const std::string TYPE;
    static const std::string EVENTTIME;
    static const std::string HOSTID;
    static const std::string RUNID;
    static const std::string STATUS;
    static const std::string TOPIC;
    static const std::string PUBTIME;

    Event();
    Event(const std::string& runid, const PropertySet::Ptr ps);
    Event(cms::TextMessage *msg, const PropertySet::Ptr ps);
    // Event(const std::string& runid, const PropertySet::Ptr& ps);

    virtual ~Event();

    // StringArray getCustomPropertyNames()
    PropertySet::Ptr getPropertySet() const;

    std::string getPubDate();
    long long getPubTime();
    void setPubTime(long long t);

    long long getEventTime();
    void setEventTime(long long nsecs);
    void updateEventTime();

    std::string getEventDate();

    std::string getHostId();
    std::string getRunId();
    std::string getType();
    std::string getStatus();

    void setTopic(std::string topic);
    std::string getTopic();

    vector<std::string> getFilterablePropertyNames();
    vector<std::string> getCustomPropertyNames();
    PropertySet::Ptr getCustomPropertySet() const;
    virtual void populateHeader(cms::TextMessage* msg) const;



protected:
    PropertySet::Ptr _psp;
    vector<std::string> _keywords;
    void _init();
    virtual void setKeywords(PropertySet::Ptr psp) const;

    long long _eventTime;
    std::string _type;
    std::string _topic;
    std::string _hostId;
    std::string _runId;
    std::string _status;
    long long _pubTime;
/*
protected:
    void init(const std::string& runId);
*/
};

}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENT_H*/
