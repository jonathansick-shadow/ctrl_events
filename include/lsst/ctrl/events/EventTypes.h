// -*- lsst-c++ -*-
/** \file EventTypes.h
  *
  * \ingroup events
  *
  * \brief defines EventTypes
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef LSST_CTRL_EVENTS_EVENTTYPES_H
#define LSST_CTRL_EVENTS_EVENTTYPES_H

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @brief Coordinate publishing and receiving events
 */
class EventTypes {
public:
    static const std::string EVENT;
    static const std::string STATUS;
    static const std::string COMMAND;
};
}
}
}

#endif /*end LSST_CTRL_EVENTS_EVENTTYPES_H*/

