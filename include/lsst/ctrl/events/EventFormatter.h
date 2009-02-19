// -*- lsst-c++ -*-
/** \file EventFormatter.h
  *
  * \ingroup events
  *
  * \brief defines the EventFormatter class
  *
  * \author Stephen Pietrowicz, NCSA
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
