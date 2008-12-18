// -*- lsst-c++ -*-
/** \class EventFormatter
  *
  * \ingroup events
  *
  * \brief EventFormatter object to support the LSST logging mechanism
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef EVENTFORMATTER_H_
#define EVENTFORMATTER_H_

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


class EventFormatter : public pexLogging::LogFormatter {
public:
    explicit EventFormatter() : pexLogging::LogFormatter() { }
    virtual ~EventFormatter();
    void write(ostream *os, const pexLogging::LogRecord& rec);

};

}
}
}
#endif
