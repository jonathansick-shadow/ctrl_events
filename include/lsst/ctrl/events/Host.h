// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2015  AURA/LSST.
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

/** 
 * @file Host.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the Host class
 *
 */

#ifndef LSST_CTRL_EVENTS_HOST_H
#define LSST_CTRL_EVENTS_HOST_H

#include <stdlib.h>

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @class Host
 * @brief This object represents the host that is being used to transmit events,
 *        giving quick access to often-used host information.
 */
class Host {
public:
    /** 
     * @brief get the Host object, which can access the host
     *         and ip address.
     * @return The EventSystem object
     */
    static Host const& getHost();

    /** 
     * @brief get the IP address
     * @return an integer representing the IP address
     */
    unsigned int const getIPAddress();

    /** 
     * @brief get the host name
     * @return a std::string containing the name of the hsot
     */
    std::string const& getHostName();

protected:
    static Host* thisHost;

    static std::string _hostname;
    static unsigned int _IPAddr;

};
}
}
}

#endif /*end LSST_CTRL_EVENTS_HOST_H*/
