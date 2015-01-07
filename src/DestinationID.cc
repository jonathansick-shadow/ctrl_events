#include <sys/types.h>
#include <unistd.h>
#include "lsst/ctrl/events/OriginatorID.h"
#include "lsst/ctrl/events/DestinationID.h"
#include "lsst/ctrl/events/Host.h"

namespace lsst {
namespace ctrl {
namespace events {

/** \brief DestinationID object. This object represents the originating process
  *        of an event.  When created, this represents a unique ID.
  */
DestinationID::DestinationID() {
    Host host = Host().getHost();
    _IPAddress = host.getIPAddress() & 0x0FFFFFFFF;
    _pid = getpid();
    _localID = _localCounter++;
}

/** \brief DestinationID object. This object represents the originating process
  *        of an event.  When created, this represents a reconstituted ID.
  */
DestinationID::DestinationID(const OriginatorID& id) {
    _constructor(id.getIPAddress(), id.getProcessID(), id.getLocalID());
}

DestinationID::DestinationID(int ipAddress, int pid, int localID) {
    _constructor(ipAddress,  pid,  localID);
}

void DestinationID::_constructor(int ipAddress, int pid, int localID) {
    _IPAddress = ipAddress;
    _pid = pid;
    _localID = localID;
}

/** \brief destructor
  */
DestinationID::~DestinationID() {
}

int DestinationID::_localCounter=0;
int DestinationID::_localID=0;
int DestinationID::_IPAddress=0;
int DestinationID::_pid=0;

int DestinationID::getIPAddress() const {
    return _IPAddress;
}

int DestinationID::getProcessID() const {
    return _pid;
}

int DestinationID::getLocalID() const {
    return _localID;
}

}
}
}
