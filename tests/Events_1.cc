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
 * @file Events_1.cc
 *
 * This tests the EventTransmitter class
 */

#include <iostream>
#include <stdexcept>
#include "lsst/ctrl/events.h"
#include "lsst/pex/exceptions.h"

#define BOOST_TEST_MODULE Events_1
#define BOOST_TEST_DYN_LINK

#include "boost/test/unit_test.hpp"

BOOST_AUTO_TEST_SUITE(Events1Suite)

namespace pexExceptions = lsst::pex::exceptions;
namespace ctrlEvents = lsst::ctrl::events;

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
    oss << "events_1_test_" << host << "_" << getpid();
    std::string topic = oss.str();


    ctrlEvents::EventTransmitter et4("lsst8.ncsa.illinois.edu", topic);

    // test publish("string", PropertySet)
    PropertySet::Ptr psp1(new PropertySet);
    psp1->set("test2",12);

    ctrlEvents::Event event1("event1", psp1);
    et4.publishEvent(event1);

    PropertySet::Ptr psp2(new PropertySet);
    psp2->set("test3",(long)13);
    ctrlEvents::Event event2("event2", psp2);
    et4.publishEvent(event2);

    // test getTopicName();
    std::string topicName = et4.getTopicName();
    BOOST_CHECK_EQUAL(topicName, topic); //Assert(topicName == topic, "Topic name does not match initial name");

}

BOOST_AUTO_TEST_SUITE_END()
