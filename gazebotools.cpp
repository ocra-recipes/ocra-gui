#include "gazebotools.h"

GazeboTools::GazeboTools(QObject *parent)
: QObject(parent)
{
    m_portName = "/OcraGui/OcraGuiPlugin/rpc:o";
    m_pluginPortName = "/Gazebo/OcraGuiPlugin/rpc:i";
    m_port.open(m_portName);

    conMon = new ConnectionMonitor(m_portName, m_pluginPortName);
    connect(conMon, SIGNAL(connected()), this, SLOT(getRobotWorldPoseInternal()) );
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

void GazeboTools::sendRobotWorldPose()
{
    emit newRobotWorldPose(robotWorldPose);
}

void GazeboTools::getRobotWorldPoseInternal()
{
    if(conMon->isConnected()) {
        yarp::os::Bottle request, reply;
        request.addString("getRobotWorldPose");
        m_port.write(request, reply);
        robotWorldPose = Eigen::Displacementd(reply.get(0).asDouble(),reply.get(1).asDouble(),reply.get(2).asDouble(),reply.get(3).asDouble(),reply.get(4).asDouble(),reply.get(5).asDouble(),reply.get(6).asDouble());
        if(reply.get(7).asBool()){
            emit showUserMessage(reply.get(8).asString(), INFO);
            emit newRobotWorldPose(robotWorldPose);
        } else {
            emit showUserMessage(reply.get(8).asString(), ERROR);
        }
    } else {
        emit showUserMessage("Not connected to Gazebo.", ERROR);
    }
}

Eigen::Displacementd GazeboTools::getRobotWorldPose()
{
    getRobotWorldPoseInternal();
    return robotWorldPose;
}
