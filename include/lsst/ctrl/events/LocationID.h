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

#include "boost/shared_ptr.hpp"

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @brief Represent process that created an event
 */
class LocationID {
public:

    typedef boost::shared_ptr<LocationID> Ptr;
    typedef boost::shared_ptr<LocationID const> ConstPtr;

    LocationID();
    LocationID(const LocationID& id);
    LocationID(const std::string& hostname, int pid, int localID);

    virtual ~LocationID();


    std::string getHostName() const;
    int getProcessID() const;
    int getLocalID() const;

protected:
    void _constructor(const std::string& hostname, int pid, int localID);
    static int _localCounter;
    int _localID;
    std::string _hostname;
    int _pid;

};
}
}
}

#endif /*end LSST_CTRL_EVENTS_LOCATIONID_H*/
