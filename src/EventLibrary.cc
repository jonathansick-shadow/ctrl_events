// -*- lsst-c++ -*-
/** \file EventLibrary.cc
  *
  * \brief Singleton used to be sure ActiveMQ is initialized.
  *
  * \ingroup events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/exceptions.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventLibrary.h"
#include <activemq/library/ActiveMQCPP.h>

namespace pexLogging =lsst::pex::logging;
namespace pexExceptions =lsst::pex::exceptions;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {
/** \brief EventLibrary makes sure that the ActiveMQ event
  *        library is initialized.  This can happen only once 
  *        per process, so this singleton is used to make sure
  *        that happens.
  */
EventLibrary::EventLibrary() {
}

/** \brief destructor
  */
EventLibrary::~EventLibrary() {
}

/** \brief initialize the ActiveMQ library, but only do it once.
  */
void EventLibrary::initializeLibrary() {
    if (libraryInitialized == 0) {
        activemq::library::ActiveMQCPP::initializeLibrary();
        libraryInitialized = 1;
    }
}
int EventLibrary::libraryInitialized = 0;

}
}
}
