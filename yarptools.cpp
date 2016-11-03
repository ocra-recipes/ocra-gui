#include "yarptools.h"

bool safeYarpConnectWithTimeout(const std::string& writePortName, const std::string& readPortName, double timeout)
{
    yarp::os::Network yarp;
    if (yarp.isConnected(writePortName, readPortName)) {
        return true;
    } else {
        double delayTime = 0.01;
        double totalDelay = 0.00;
        bool connected = yarp.connect(writePortName, readPortName);
        while ( !connected && totalDelay <= timeout ) {
            connected = yarp.connect(writePortName, readPortName);
            yarp::os::Time::delay(0.01);
            totalDelay += delayTime;
        }
        if (!connected) {
            std::string s = "Couldn't connect " + writePortName + " to " + readPortName;
//            MESSAGE_TYPE t = WARNING;
//            emit showUserMessage(s, t);
            std::cout << s << std::endl;
        }
        return connected;
    }
}
