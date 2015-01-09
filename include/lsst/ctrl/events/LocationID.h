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
 
/** \file LocationID.h
  *
  * \ingroup events
  *
  * \brief defines the LocationID class
  *
  */

#ifndef LSST_CTRL_EVENTS_LOCATIONID_H
#define LSST_CTRL_EVENTS_LOCATIONID_H

#include <stdlib.h>
#include <iostream>

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @brief Represent process that created an event
 */
class LocationID {
public:
    LocationID();
    LocationID(const LocationID& id);
    LocationID(const std::string& hostname, int pid, int localID);

    ~LocationID();

    std::string getHostName() const;
    int getProcessID() const;
    int getLocalID() const;

protected:
    void _constructor(const std::string& hostname, int pid, int localID);
    static int _localCounter;
    static int _localID;
    static std::string _hostname;
    static int _pid;

};
}
}
}

#endif /*end LSST_CTRL_EVENTS_LOCATIONID_H*/
