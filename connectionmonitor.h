#ifndef CONNECTIONMONITOR_H
#define CONNECTIONMONITOR_H

#include <QObject>
#include <QTimer>
#include "yarptools.h"
#include "messagetypes.h"
class ConnectionMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionMonitor(const std::string& outPortName, const std::string& inPortName, QObject *parent = 0);
    virtual ~ConnectionMonitor();
    bool isConnected();

signals:
    void connected();
    void disconnected();
    void showUserMessage(const std::string& s, MESSAGE_TYPE t);

public slots:
    void checkConnection();

private:
    std::string m_outPortName;
    std::string m_inPortName;

    QTimer *m_connectionTimer;

    bool m_isConnected;
    int m_fastConnectCheckTime;
    int m_slowConnectCheckTime;

    yarp::os::Network m_yarp;
};

#endif // CONNECTIONMONITOR_H
