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
 * @file Host.cc
 *
 * @ingroup ctrl/events
 *
 * @brief quickly return the host ip address
 *
 */
#include <limits.h>
#include <string>
#include <string.h>
#include <netdb.h>

#include "lsst/ctrl/events/Host.h"

namespace lsst {
namespace ctrl {
namespace events {

Host const& Host::getHost() {
    if (thisHost == 0) {

        // create the _IPAddr here, rather than
        // reconstructing it every time we create an
        // identificationId

        char buf [HOST_NAME_MAX];
        struct hostent *ent;
        unsigned char a,b,c,d;

        gethostname(buf, HOST_NAME_MAX);
        _hostname.assign(buf,strlen(buf));
        ent = (struct hostent *)gethostbyname(buf) ;

        a = ent->h_addr_list[0][0] & 0xFF;
        b = ent->h_addr_list[0][1] & 0xFF;
        c = ent->h_addr_list[0][2] & 0xFF;
        d = ent->h_addr_list[0][3] & 0xFF;

        _IPAddr = (a << 24) | (b << 16) | (c << 8) | d;
  
        // create the default EventSystem object
        thisHost = new Host();
    }
    return *thisHost;

}

Host* Host::thisHost = 0;
unsigned int Host::_IPAddr = 0;
std::string Host::_hostname;

unsigned int const Host::getIPAddress() {
    return _IPAddr;
}

std::string const& Host::getHostName() {
    return _hostname;
}

}
}
}
