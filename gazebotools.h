#ifndef GAZEBOTOOLS_H
#define GAZEBOTOOLS_H

#include <QObject>
#include <yarp/os/all.h>
#include <QTimer>
#include "yarptools.h"
#include "messagetypes.h"
#include "connectionmonitor.h"

class GazeboTools : public QObject
{
    Q_OBJECT

public:
    explicit GazeboTools(QObject *parent = 0);
    virtual ~GazeboTools();

signals:
    void showUserMessage(const std::string& msg, MESSAGE_TYPE type);

public slots:
    void addTaskFrames(const std::string &taskName);
    void removeTaskFrames(const std::string &taskName);

private:
    yarp::os::Network yarp;
    yarp::os::RpcClient m_port;
    ConnectionMonitor *conMon;
    std::string m_portName;
    std::string m_pluginPortName;
};

#endif // GAZEBOTOOLS_H


