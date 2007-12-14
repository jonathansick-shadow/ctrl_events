// -*- lsst-c++ -*-
/** \file EventFormatter.h
  *
  * \ingroup events
  *
  * \brief 
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
    /**
      * \brief constructor for EventLog.   Default values are:
      *        threshold is Log::INFO, with a null preamble
      */
    EventLog(int threshold=Log::INFO, 
             const vector<shared_ptr<DataProperty> > *preamble=0);

    virtual ~EventLog();

    /**
      * \brief get the current event threshold
      * \return the event threshold
      */
    int getEventThreshold() { return _log->getThreshold(); }

    /**
      * \brief set the event threshold
      * \param the threshold to set for the log destination
      */
    void setEventThreshold(int thres) {_log->setThreshold(thres); }


    static const std::string getLoggingTopic() {
        return "LSSTLogging";
    }

    static void createDefaultLog(int threshold=Log::INFO, 
             const vector<shared_ptr<DataProperty> > *preamble=0);

private:
    LogDestination *_log;
    EventFormatter *_formatter;
};

}
}
#endif
