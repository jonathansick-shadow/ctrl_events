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

using lsst::daf::base::PropertySet;

using namespace lsst::daf::base;
using namespace lsst::pex::logging;
using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

class EventFormatter : public LogFormatter {
public:
    explicit EventFormatter() : LogFormatter() { }
    virtual ~EventFormatter();
    void write(ostream *os, const LogRecord& rec);

};

}
}
}
#endif
