#include <sys/types.h>
#include <unistd.h>
#include "lsst/ctrl/events/OriginatorID.h"
#include "lsst/ctrl/events/Host.h"

namespace lsst {
namespace ctrl {
namespace events {

/** \brief OriginatorID object. This object represents the originating process
  *        of an event.  When created, this represents a unique ID.
  */
OriginatorID::OriginatorID() {
    Host host = Host().getHost();
    _IPAddress = host.getIPAddress() & 0x0FFFFFFFF;
    _pid = getpid();
    _localID = _localCounter++;
}

/** \brief OriginatorID object. This object represents the originating process
  *        of an event.  When created, this represents a reconstituted ID.
  */
OriginatorID::OriginatorID(int ipAddress, int pid, int localID) {
    _IPAddress = ipAddress;
    _pid = pid;
    _localID = localID;
}

/** \brief destructor
  */
OriginatorID::~OriginatorID() {
}

int OriginatorID::_localCounter=0;
int OriginatorID::_localID=0;
int OriginatorID::_IPAddress=0;
int OriginatorID::_pid=0;

int OriginatorID::getIPAddress() const {
    return _IPAddress;
}

int OriginatorID::getProcessID() const {
    return _pid;
}

int OriginatorID::getLocalID() const {
    return _localID;
}

}
}
}
