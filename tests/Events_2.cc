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
 * @file Events_2.cc
 *
 * This tests the EventReceiver class
 */
#include <iostream>
#include <stdexcept>
#include "lsst/ctrl/events.h"
#include "lsst/pex/exceptions.h"
using namespace std;
using lsst::pex::policy::Policy;

using lsst::pex::exceptions::NotFoundError;
using lsst::pex::exceptions::RuntimeError;

using lsst::ctrl::events::EventReceiver;

#define BOOST_TEST_MODULE Events2
#define BOOST_TEST_DYN_LINK

#include "boost/test/unit_test.hpp"

BOOST_AUTO_TEST_SUITE(EventReceiveSuite)

#define Assert(b, m) tattle(b, m, __LINE__)

void tattle(bool mustBeTrue, const string& failureMsg, int line) {
    if (! mustBeTrue) {
        ostringstream msg;
        msg << __FILE__ << ':' << line << ":\n" << failureMsg << ends;
        throw LSST_EXCEPT(RuntimeError, msg.str());
    }
}

BOOST_AUTO_TEST_CASE(all) {

    Policy p;
    std::ostringstream oss;
    char host[128];

    int ret = gethostname(host,sizeof(host));
    if (ret != 0)
        throw runtime_error("error getting hostname");
    oss << "events_2_test_" << host << "_" << getpid();
    std::string topic = oss.str();

    //
    // test EventReceiver(const Policy& policy)
    //
    try {
        EventReceiver er1(p);
    } catch (NotFoundError&) {
    }



    p.set("topicName", topic);
    try {
        EventReceiver er2(p);
    } catch (NotFoundError&) {
    }

    p.set("topicName", topic);
    p.set("hostName", "garbage");
    try {
        EventReceiver er2(p);
    } catch (RuntimeError&) {
    }

    p.set("topicName", topic);
    p.set("hostName", "lsst8.ncsa.illinois.edu");
    try {
        EventReceiver er2(p);
    } catch (RuntimeError&) {
    }

    EventReceiver er3("lsst8.ncsa.illinois.edu", topic);

    // test getTopicName();
    std::string topicName = er3.getTopicName();
    BOOST_CHECK_EQUAL(topicName, topic); //Assert(topicName == topic, "Topic name does not match initial name");

}

BOOST_AUTO_TEST_SUITE_END()
