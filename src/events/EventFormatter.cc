// -*- lsst-c++ -*-
/** \file EventFormatter.cc
  *
  * \brief
  *
  * Author: Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/events/EventSystem.h"
#include "lsst/events/EventLog.h"
#include "lsst/events/EventFormatter.h"
#include "lsst/mwi/data/DataProperty.h"
#include "lsst/mwi/data/SupportFactory.h"
#include "lsst/mwi/logging/LogRecord.h"
#include "lsst/mwi/utils/Component.h"

using namespace lsst::mwi::data;
using namespace lsst::mwi::logging;

using namespace std;

namespace lsst {
namespace events {

    /**
      * \brief writes a record to the event log stream.   This ignores the
      *        ostream, but this is required because of the signature of this
      *        required method
      * \param os output stream.  this is ignored, since the output stream
      *           is the event stream; it's a placeholder because of the
      *           signature of this method.
      * \param rec the LogRecord to send to the event topic;
      */
    void EventFormatter::write(ostream *os, const LogRecord& rec) {
        EventSystem system = EventSystem::getDefaultEventSystem();
        system.publish(EventLog::getLoggingTopic(), rec);
    }

}
}
