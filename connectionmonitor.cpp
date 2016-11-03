#include "connectionmonitor.h"

ConnectionMonitor::ConnectionMonitor(const std::string &outPortName, const std::string &inPortName, QObject *parent)
: m_outPortName(outPortName)
, m_inPortName(inPortName)
, QObject(parent)
, m_isConnected(false)
, m_fastConnectCheckTime(500)
, m_slowConnectCheckTime(1000)
{
    m_connectionTimer = new QTimer();
    connect(m_connectionTimer, SIGNAL(timeout()), this, SLOT(checkConnection()) );
    m_connectionTimer->start(m_fastConnectCheckTime);
}

ConnectionMonitor::~ConnectionMonitor()
{
    delete m_connectionTimer;
}

bool ConnectionMonitor::isConnected()
{
    return m_isConnected;
}

void ConnectionMonitor::checkConnection()
{
    if(m_isConnected) {
        if(!m_yarp.isConnected(m_outPortName, m_inPortName)) {
            m_isConnected = false;
            emit disconnected();
            m_connectionTimer->setInterval(m_fastConnectCheckTime);
            emit showUserMessage("Lost connection with Gazebo.", WARNING);
        }
    } else {
        if(m_yarp.exists(m_outPortName) && m_yarp.exists(m_inPortName)) {
            if(safeYarpConnectWithTimeout(m_outPortName, m_inPortName, 1.0)) {
                m_isConnected = true;
                emit connected();
                m_connectionTimer->setInterval(m_slowConnectCheckTime);
            }
        }
    }
}
