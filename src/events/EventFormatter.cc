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


void EventFormatter::write(ostream *os, const LogRecord& rec) {
    EventSystem system = EventSystem::getDefaultEventSystem();
    system.publish(EventLog::getLoggingTopic(), rec);
}

}
}
