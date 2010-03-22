// -*- lsst-c++ -*-
/** \file EventLibrary.h
  *
  * \ingroup events
  *
  * \brief defines the EventLibrary class
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef LSST_CTRL_EVENTS_EVENTLIBRARY_H
#define LSST_CTRL_EVENTS_EVENTLIBRARY_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/ctrl/events.h"

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {
/**
 * @brief Singleton use to make sure the events library is initialized.
 */
class EventLibrary {
public:
    EventLibrary();

    ~EventLibrary();

    static void initializeLibrary();

protected:
    static int libraryInitialized;
};
}
}
}

#endif /*end LSST_CTRL_EVENTS_EVENTLIBRARY_H*/

