// -*- lsst-c++ -*-
/** \class EventLog
  *
  * \ingroup events
  *
  * \brief EventLog object used by the LSST logging system
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef EVENTLOG_H_
#define EVENTLOG_H_

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/EventFormatter.h"
#include "lsst/pex/logging/Log.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;

using namespace lsst::daf::base;
using namespace lsst::pex::logging;
using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

class EventLog : public Log {
public:
    EventLog( const std::string runId, int sliceId, const std::string hostId = "", int threshold=Log::INFO);
    EventLog( const std::string runId, int sliceId, const PropertySet::Ptr& preamble, const std::string hostId = "", int threshold=Log::INFO);

    virtual ~EventLog();

    int getEventThreshold() { return _log->getThreshold(); }

    void setEventThreshold(int thres) {_log->setThreshold(thres); }


    /** \brief return the static name used as a logging topic
      */
    static const std::string getLoggingTopic() {
        return "LSSTLogging";
    }

    static void createDefaultLog(const std::string runId, int slideId, const std::string hostId = "", int threshold=Log::INFO); 
    static void createDefaultLog(const std::string runId, int slideId, const PropertySet::Ptr& preamble, const std::string hostId = "", int threshold=Log::INFO); 

private:
    LogDestination *_log;
    EventFormatter *_formatter;
    void init(const std::string runId, int sliceId, const PropertySet::Ptr& preamble, const std::string hostId, int threshold);
    void initThres(int threshold);
};

}
}
}
#endif
