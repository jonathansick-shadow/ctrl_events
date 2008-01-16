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

#include "lsst/mwi/logging/LogRecord.h"
#include "lsst/mwi/logging/LogFormatter.h"
#include "lsst/mwi/utils/Component.h"
#include "lsst/mwi/utils/Utils.h"
#include "lsst/mwi/data/DataProperty.h"

using lsst::mwi::data::DataProperty;

using namespace lsst::mwi::data;
using namespace lsst::mwi::logging;
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
