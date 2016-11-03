#ifndef YARPTOOLS_H
#define YARPTOOLS_H

#include <yarp/os/all.h>
#include "messagetypes.h"

bool safeYarpConnectWithTimeout(const std::string& writePortName, const std::string& readPortName, double timeout=1.0);

#endif // YARPTOOLS_H
