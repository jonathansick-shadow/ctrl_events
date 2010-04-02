// -*- lsst-c++ -*-
/** \file EventTypes.cc
  *
  * \brief Names used in events
  *
  * \ingroup events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */

#include <iomanip>
#include "lsst/ctrl/events/EventTypes.h"

namespace lsst {
namespace ctrl {
namespace events {
    const std::string EventTypes::EVENT = "EVENT";
    const std::string EventTypes::STATUS = "STATUS";
    const std::string EventTypes::COMMAND = "COMMAND";
}
}
}
