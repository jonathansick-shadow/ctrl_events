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
 * @file LocationId.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the LocationId class
 *
 */

#ifndef LSST_CTRL_EVENTS_LOCATIONID_H
#define LSST_CTRL_EVENTS_LOCATIONID_H

#include <stdlib.h>

#include "boost/shared_ptr.hpp"

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @class LocationId
 * @brief Represent process that created an event
 */
class LocationId {
public:

    typedef boost::shared_ptr<LocationId> Ptr;
    typedef boost::shared_ptr<LocationId const> ConstPtr;

    /** 
     * @brief LocationId object. This object represents the originating process
     *        process of an event. This consists of the host name, the process 
     *        id of the process that created this LocationId and a local id 
     *        which is unique for this process. When created, this represents 
     *        a unique location ID.
     */
    LocationId();

    /** 
     * @brief LocationId object. This object represents the originating process
     *        of an event.  When created, this represents a duplicated ID.
     */
    LocationId(LocationId const& id);

    /** 
     * @brief LocationId object. This object represents the originating process
     *        of an event.  When created, this represents a reconstituted ID.
     */
    LocationId(std::string const& hostname, int pid, int localID);


    /** 
     * @brief Retrieve the host name
     * @return a string containing the name of a host
     */
    std::string getHostName() const;

    /** 
     * @brief Retrieve the process id
     * @return an int of the process id
     */
    int getProcessID() const;

    /** 
     * @brief Retrieve the local id
     * @return an int of local id
     */
    int getLocalID() const;

protected:
    static int _localCounter; /// used for localID & is unique for instances of LocationId in a process
    std::string _hostname;    /// host name
    int _pid;                 /// process id
    int _localID;             /// local id

};
}
}
}

#endif /*end LSST_CTRL_EVENTS_LOCATIONID_H*/
