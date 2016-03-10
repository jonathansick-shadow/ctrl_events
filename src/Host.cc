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
#include "boost/scoped_array.hpp"
#include <ifaddrs.h>
#include <unistd.h>

#include "lsst/ctrl/events/Host.h"

#include "lsst/pex/exceptions.h"

namespace pexExceptions = lsst::pex::exceptions;

namespace lsst {
namespace ctrl {
namespace events {

Host const& Host::getHost() {
    if (thisHost == 0) {

        // create the _hostname here, rather than
        // reconstructing it every time we create an
        // identificationId

        long int host_len = sysconf(_SC_HOST_NAME_MAX)+1; // add one for the null

        std::vector<char> vec;
        vec.resize(host_len);

        if (gethostname(vec.data(), vec.size()) == 0) {
            _hostname = std::string(vec.data());
        } else {
            std::string msg("call to gethostname() failed");
            throw LSST_EXCEPT(pexExceptions::RuntimeError, msg);
        }

        // create the default EventSystem object
        thisHost = new Host();
    }
    return *thisHost;

}

Host* Host::thisHost = 0;
std::string Host::_hostname;

std::string const& Host::getHostName() {
    return _hostname;
}

}}}
