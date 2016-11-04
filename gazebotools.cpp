#include "gazebotools.h"

GazeboTools::GazeboTools(QObject *parent)
: QObject(parent)
{
    m_portName = "/OcraGui/OcraGuiPlugin/rpc:o";
    m_pluginPortName = "/Gazebo/OcraGuiPlugin/rpc:i";
    m_port.open(m_portName);

    conMon = new ConnectionMonitor(m_portName, m_pluginPortName);
}

GazeboTools::~GazeboTools()
{

}

void GazeboTools::addTaskFrames(const std::string& taskName)
{
    if(conMon->isConnected()) {
        yarp::os::Bottle request, reply;
        request.addString("addTaskFrames");
        request.addString(taskName);
        m_port.write(request, reply);
        if(reply.get(0).asBool()){
            emit showUserMessage(reply.get(1).asString(), INFO);
        } else {
            emit showUserMessage(reply.get(1).asString(), ERROR);
        }
    } else {
        emit showUserMessage("Not connected to Gazebo.", ERROR);
    }
}

void GazeboTools::removeTaskFrames(const std::string& taskName)
{
    if(conMon->isConnected()) {
        yarp::os::Bottle request, reply;
        request.addString("removeTaskFrames");
        request.addString(taskName);
        m_port.write(request, reply);
        if(reply.get(0).asBool()){
            emit showUserMessage(reply.get(1).asString(), INFO);
        } else {
            emit showUserMessage(reply.get(1).asString(), ERROR);
        }
    } else {
        emit showUserMessage("Not connected to Gazebo.", ERROR);
    }
}
