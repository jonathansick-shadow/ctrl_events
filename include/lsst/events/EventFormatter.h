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
#include "lsst/pex/utils/Component.h"
#include "lsst/pex/utils/Utils.h"
#include "lsst/daf/data/DataProperty.h"

using lsst::daf::data::DataProperty;

using namespace lsst::daf::data;
using namespace lsst::pex::logging;
using namespace std;

namespace lsst {
namespace events {

class EventFormatter : public LogFormatter {
public:
    explicit EventFormatter() : LogFormatter() { }
    void write(ostream *os, const LogRecord& rec);

};

}
}
#endif
