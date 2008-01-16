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

#include "lsst/events/EventFormatter.h"
#include "lsst/mwi/logging/Log.h"
#include "lsst/mwi/utils/Component.h"
#include "lsst/mwi/utils/Utils.h"
#include "lsst/mwi/data/DataProperty.h"

using lsst::mwi::data::DataProperty;

using namespace lsst::mwi::data;
using namespace lsst::mwi::logging;
using namespace std;

namespace lsst {
namespace events {

class EventLog : public Log {
public:
    EventLog(const std::string runId, int sliceId, const std::string hostId = "", int threshold=Log::INFO, const list<shared_ptr<DataProperty> > *preamble = 0);

    virtual ~EventLog();

    int getEventThreshold() { return _log->getThreshold(); }

    void setEventThreshold(int thres) {_log->setThreshold(thres); }


    /** \brief return the static name used as a logging topic
      */
    static const std::string getLoggingTopic() {
        return "LSSTLogging";
    }

    static void createDefaultLog(const std::string runId, int slideId, const std::string hostId = "", int threshold=Log::INFO, 
             const list<shared_ptr<DataProperty> > *preamble=0);

private:
    LogDestination *_log;
    EventFormatter *_formatter;
    void init(int threshold);
};

}
}
#endif
