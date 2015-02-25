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

/**
 * @file EventSystem_2.cc
 *
 * This tests the EventSystem class
 */
#include <iostream>
#include <stdexcept>
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/pex/exceptions.h"

using namespace std;
using lsst::pex::exceptions::NotFoundError;
using lsst::pex::exceptions::RuntimeError;

using lsst::ctrl::events::EventSystem;

#define BOOST_TEST_MODULE EventSystem2
#define BOOST_TEST_DYN_LINK
#include "boost/test/unit_test.hpp"

BOOST_AUTO_TEST_SUITE(EventSystem2)

#define Assert(b, m) tattle(b, m, __LINE__)

void tattle(bool mustBeTrue, const string& failureMsg, int line) {
    if (! mustBeTrue) {
        ostringstream msg;
        msg << __FILE__ << ':' << line << ":\n" << failureMsg << ends;
        throw runtime_error(msg.str());
    }
}

BOOST_AUTO_TEST_CASE(all) {

    std::ostringstream oss;
    char host[128];

    int ret = gethostname(host,sizeof(host));
    if (ret != 0)
        throw runtime_error("error getting hostname");
    oss << "eventsystem_2_test_" << host << "_" << getpid();
    std::string topic = oss.str();

    EventSystem eventSystem = EventSystem::getDefaultEventSystem();

    eventSystem.createReceiver("lsst8.ncsa.illinois.edu", topic);
    try {
        eventSystem.createReceiver("lsst8.ncsa.illinois.edu", topic);
        // should never get here
        BOOST_CHECK_EQUAL(0,1);
    } catch (RuntimeError&) {
        // can't create a receiver if one already exists for that topic
    }
    oss.str("");
    oss << "eventsystem_2a_test_" << host << "_" << getpid();
    std::string topic2 = oss.str();
    eventSystem.createReceiver("lsst8.ncsa.illinois.edu", topic2);
}

BOOST_AUTO_TEST_SUITE_END()
