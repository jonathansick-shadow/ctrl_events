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
    const std::string EventTypes::EVENT = "_E";
    const std::string EventTypes::LOG = "_L";
    const std::string EventTypes::STATUS = "_S";
    const std::string EventTypes::COMMAND = "_C";
    const std::string EventTypes::PIPELINELOG = "_P";
}
}
}
