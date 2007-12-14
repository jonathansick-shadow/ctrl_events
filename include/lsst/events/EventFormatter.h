// -*- lsst-c++ -*-
/** \file EventFormatter.h
  *
  * \ingroup events
  *
  * \brief 
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
    /**
      * \brief Logging formatter for events
      *
      */
    explicit EventFormatter() : LogFormatter() { }

    /**
      * \brief writes a record to the event log stream.   This ignores the
      *        ostream, but this is required because of the signature of this
      *        required method
      */
    void write(ostream *os, const LogRecord& rec);

};

}
}
#endif
