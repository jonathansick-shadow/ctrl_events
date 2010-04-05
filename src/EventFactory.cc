// -*- lsst-c++ -*-
/** \file EventFactory.cc
  *
  * \brief Coordinate EventTransmitters and EventReceiver objects
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

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/EventTypes.h"

#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFactory.h"
#include "lsst/ctrl/events/EventLibrary.h"

namespace pexLogging =lsst::pex::logging;
namespace pexExceptions =lsst::pex::exceptions;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {
/** \brief EventFactory object.  This object creates Event objects
  *        from TextMessages.
  */
EventFactory::EventFactory() {
}

/** \brief destructor
  */
EventFactory::~EventFactory() {
}

/** \brief return an Event object, based on the type received in the TextMessage.
  * \return An Event object
  */
Event* EventFactory::createEvent(cms::TextMessage* msg, const PropertySet::Ptr psp) {
    vector<std::string> names = msg->getPropertyNames();

    /*
    for (unsigned int i = 0; i < names.size(); i++) 
        std::cout << names[i] << std::endl;
    */

    std::string _type = msg->getStringProperty("TYPE");
    if (_type == EventTypes::STATUS) {
        return new StatusEvent(msg, psp);
    } else if (_type == EventTypes::COMMAND) {
        return new CommandEvent(msg, psp);
    }
    return new Event(msg, psp);
}
}

}
}
