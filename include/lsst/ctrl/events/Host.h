// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
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
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
/** \file Host.h
  *
  * \ingroup events
  *
  * \brief defines the Host class
  *
  */

#ifndef LSST_CTRL_EVENTS_HOST_H
#define LSST_CTRL_EVENTS_HOST_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @brief Coordinate publishing and receiving events
 */
class Host {
public:
    Host();

    ~Host();

    static Host& getHost();

    unsigned int getIPAddress();
    std::string getHostName();

protected:
    static Host *thisHost;

    static std::string _hostname;
    static unsigned int _IPAddr;

};
}
}
}

#endif /*end LSST_CTRL_EVENTS_EVENTSYSTEM_H*/

