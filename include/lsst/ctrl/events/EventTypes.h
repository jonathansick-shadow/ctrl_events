// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2014  AURA/LSST.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */

/** \file EventTypes.h
  *
  * \ingroup events
  *
  * \brief defines EventTypes
  *
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
    static const std::string LOG;
    static const std::string STATUS;
    static const std::string COMMAND;
};
}
}
}

#endif /*end LSST_CTRL_EVENTS_EVENTTYPES_H*/

