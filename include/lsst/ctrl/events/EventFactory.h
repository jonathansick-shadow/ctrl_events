// -*- lsst-c++ -*-
/** \file EventFactory.h
  *
  * \ingroup events
  *
  * \brief defines the EventFactory class
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef LSST_CTRL_EVENTS_EVENTFACTORY_H
#define LSST_CTRL_EVENTS_EVENTFACTORY_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/ctrl/events.h"

using lsst::daf::base::PropertySet;
namespace pexPolicy = lsst::pex::policy;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @brief create LSST Events from JMS Messages
 */
class EventFactory {
public:
    EventFactory();

    ~EventFactory();

    static Event* createEvent(cms::TextMessage* msg);

};
}
}
}

#endif /*end LSST_CTRL_EVENTS_EVENTFACTORY_H*/

