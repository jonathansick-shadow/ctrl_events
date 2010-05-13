// -*- lsst-c++ -*-
/** \file EventBroker.h
  *
  * \ingroup events
  *
  * \brief defines information pertaining to the Event Broker
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef LSST_CTRL_EVENTS_EVENTBROKER_H
#define LSST_CTRL_EVENTS_EVENTBROKER_H

namespace lsst {
namespace ctrl {
namespace events {

class EventBroker {
public:
    static const int DEFAULTHOSTPORT = 61616;
};

}
}
}

#endif /*end LSST_CTRL_EVENTS_EVENTBROKER_H*/

