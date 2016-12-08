#include "taskwidget.h"
#include "ui_taskwidget.h"

TaskWidget::TaskWidget(const std::string& name, QWidget *parent)
: m_name(name)
, QWidget(parent)
, ui(new Ui::TaskWidget)
, m_weight(0.0)
, m_level(-1)
, m_kp(0.0)
, m_kd(0.0)
, currentlyEditing(false)
{
    ui->setupUi(this);
    taskCon = std::make_shared<ocra_recipes::TaskConnection>(m_name);
    taskCon->openControlPorts(false);
    m_type = taskCon->getTaskTypeAsString();
    ui->taskTypeLabel->setText(m_type.c_str());
    ui->taskNameLabel->setText(m_name.c_str());


    if ( !((m_type=="position"      ) ||
           (m_type=="orientation"   ) ||
           (m_type=="pose"          ) ||
           (m_type=="force"         ) ||
           (m_type=="com"           ) ||
           (m_type=="point_contact" )
          )
        )
    {
        ui->gazeboButton->setEnabled(false);
    }

    if ( (m_type=="point_contact"      ) ||
         (m_type=="unknown"   )
       )
    {
        ui->weightLineEdit->setEnabled(false);
        ui->levelLineEdit->setEnabled(false);
        ui->kpLineEdit->setEnabled(false);
        ui->kdLineEdit->setEnabled(false);
        if (m_type=="unknown"   ) {
            ui->currentStateLineEdit->setEnabled(false);
        }
        ui->desiredStateLineEdit->setEnabled(false);

    }

    if (taskCon->isActivated()) {
        ui->activateButton->setChecked(true);
    }

    refreshParameters();

    yarp::os::Bottle reply;
    taskCon->queryTask(ocra::TASK_MESSAGE::GET_CONTROL_PORT_NAMES, reply);
    if(reply.size() == 3) {
        taskStatePortOutName = reply.get(1).asString();
        taskDesiredStatePortOutName = reply.get(2).asString();
    }

    framePortOutName = "/OcraGui/TaskWidget/"+m_name+"-Frame"+":o";
    targetPortOutName = "/OcraGui/TaskWidget/"+m_name+"-Target"+":o";
    framePortInName = "/Gazebo/"+m_name+"-Frame"+":i";
    targetPortInName = "/Gazebo/"+m_name+"-Target"+":i";

    taskStatePortInName = "/OcraGui/TaskWidget/"+m_name+"/state:i";
    taskDesiredStatePortInName = "/OcraGui/TaskWidget/"+m_name+"/desired_state:i";


    framePortOut.open(framePortOutName);
    targetPortOut.open(targetPortOutName);
    taskStateInPort.open(taskStatePortInName);
    taskDesiredStateInPort.open(taskDesiredStatePortInName);



    m_currentStatePortReadTimer = new QTimer(this);
    connect(m_currentStatePortReadTimer, SIGNAL(timeout()), this, SLOT(readCurrentStatePort()));


    m_desiredStatePortReadTimer = new QTimer(this);
    connect(m_desiredStatePortReadTimer, SIGNAL(timeout()), this, SLOT(readDesiredStatePort()));



    connect(this, SIGNAL(currentStateUpdated()), this, SLOT(updateCurrentState()));
    connect(this, SIGNAL(desiredStateUpdated()), this, SLOT(updateDesiredState()));

    connectPorts();

    connect(ui->desiredStateLineEdit, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(disconnectDesiredStateInput()));
    connect(ui->desiredStateLineEdit, SIGNAL(editingFinished()), this, SLOT(sendAndReconnectDesiredStateInput()));
}

void TaskWidget::disconnectDesiredStateInput()
{
    if(!currentlyEditing) {
        disconnect(this, SIGNAL(desiredStateUpdated()), this, SLOT(updateDesiredState()));
        currentlyEditing = true;
    }
}

void TaskWidget::sendAndReconnectDesiredStateInput()
{
    Eigen::VectorXd desVec = ocra::util::stringToVectorXd(ui->desiredStateLineEdit->text().toStdString().c_str());
    Eigen::Displacementd dispCurrent = desiredState.getPosition();

    dispCurrent.x() = desVec(0);
    dispCurrent.y() = desVec(1);
    dispCurrent.z() = desVec(2);

    ocra::TaskState newState;
    newState.setPosition(dispCurrent);

    taskCon->setDesiredTaskState(newState);
    connect(this, SIGNAL(desiredStateUpdated()), this, SLOT(updateDesiredState()));
    currentlyEditing = false;
}

TaskWidget::~TaskWidget()
{
    if (ui->gazeboButton->isChecked()) {
        emit removeGazeboFrames(m_name);
    }
    framePortOut.close();
    targetPortOut.close();
    taskStateInPort.close();
    taskDesiredStateInPort.close();
    delete m_currentStatePortReadTimer;
    delete m_desiredStatePortReadTimer;
    delete currentStateBottle;
    delete desiredStateBottle;
    delete ui;
}

void TaskWidget::connectPorts()
{
    emit showUserMessage("Connecting task widget ports for task: " + m_name, INFO);
    taskCon->reconnect();
    taskCon->openControlPorts(false);
    bool allConnected = true;
    if (ui->gazeboButton->isChecked()) {
        allConnected &= connectToGazebo();
    }
    allConnected &= safeYarpConnectWithTimeout(taskStatePortOutName, taskStatePortInName);
    allConnected &= safeYarpConnectWithTimeout(taskDesiredStatePortOutName, taskDesiredStatePortInName);
    if (allConnected && !m_currentStatePortReadTimer->isActive() && !m_desiredStatePortReadTimer->isActive()) {
        int portReadLoopTime = 5;
        m_currentStatePortReadTimer->start(portReadLoopTime);
        m_desiredStatePortReadTimer->start(portReadLoopTime);
    }
}

void TaskWidget::disconnectPorts()
{
    taskCon->disconnect();
    yarp.disconnect(taskStatePortOutName, taskStatePortInName);
    yarp.disconnect(taskDesiredStatePortOutName, taskDesiredStatePortInName);
    emit showUserMessage("Disconnecting task widget ports for task: " + m_name, INFO);
    m_currentStatePortReadTimer->stop();
    m_desiredStatePortReadTimer->stop();
}

bool TaskWidget::connectToGazebo()
{
    bool allConnected = true;
    allConnected &= safeYarpConnectWithTimeout(framePortOutName, framePortInName);
    allConnected &= safeYarpConnectWithTimeout(targetPortOutName, targetPortInName);
    return allConnected;
}

void TaskWidget::updateCurrentState()
{
    if (   (m_type=="position"      ) ||
           (m_type=="orientation"   ) ||
           (m_type=="pose"          ) ||
           (m_type=="force"         ) ||
           (m_type=="com"           ) ||
           (m_type=="point_contact" )
       )
    {
        if (ui->gazeboButton->isChecked()) {
            sendPosesToGazebo();
        } else {
            displacementToXYZRPY(currentState.getPosition(), framePose);
        }

//        ui->currentStateLineEdit->setText(poseToString(framePose));
        ui->currentStateLineEdit->setText(displacementToString(currentState.getPosition()));

    }
}


void TaskWidget::updateDesiredState()
{
    if (   (m_type=="position"      ) ||
           (m_type=="orientation"   ) ||
           (m_type=="pose"          ) ||
           (m_type=="force"         ) ||
           (m_type=="com"           ) ||
           (m_type=="point_contact" )
       )
    {
        if (ui->gazeboButton->isChecked()) {
            sendPosesToGazebo();
        } else {
            displacementToXYZRPY(desiredState.getPosition(), targetPose);
        }

//        ui->desiredStateLineEdit->setText(poseToString(targetPose));
        ui->desiredStateLineEdit->setText(displacementToString(desiredState.getPosition()));
    }
}


QString TaskWidget::poseToString(const std::vector<double>& poseVector)
{
    QString s("");
    for (auto i : poseVector) {
        s.append(QString::number(i, 'g', 3));
        s.append(" ");
    }
    return s;
}

QString TaskWidget::displacementToString(const Eigen::Displacementd& disp)
{
    QString s("");
    s.append(QString::number(disp.x(), 'g', 3));
    s.append(" ");
    s.append(QString::number(disp.y(), 'g', 3));
    s.append(" ");
    s.append(QString::number(disp.z(), 'g', 3));
    s.append(" ");
    s.append(QString::number(disp.qw(), 'g', 3));
    s.append(" ");
    s.append(QString::number(disp.qx(), 'g', 3));
    s.append(" ");
    s.append(QString::number(disp.qy(), 'g', 3));
    s.append(" ");
    s.append(QString::number(disp.qz(), 'g', 3));
    s.append(" ");
    return s;
}

void TaskWidget::on_activateButton_clicked(bool checked)
{
    if (checked) {
        ui->deactivateButton->setChecked(false);
        taskCon->activate();
    } else {
        ui->activateButton->setChecked(true);
    }
}

void TaskWidget::on_deactivateButton_clicked(bool checked)
{
    if (checked) {
        ui->activateButton->setChecked(false);
        taskCon->deactivate();
    } else {
        ui->deactivateButton->setChecked(true);
    }
}

void TaskWidget::on_gazeboButton_clicked(bool checked)
{
    if (checked) {
        emit getRobotWorldPose();
        emit addGazeboFrames(m_name);
        if(!connectToGazebo()) {
            ui->gazeboButton->setChecked(false);
        }
    } else {
        emit removeGazeboFrames(m_name);
    }
}

void TaskWidget::updateParameterValues()
{
    ui->weightLineEdit->setText(QString::number(m_weight));
    ui->levelLineEdit->setText(QString::number(m_level));
    ui->kpLineEdit->setText(QString::number(m_kp));
    ui->kdLineEdit->setText(QString::number(m_kd));
}

void TaskWidget::sendPosesToGazebo()
{
    frameOutputBottle.clear();
    targetOutputBottle.clear();

    Eigen::Displacementd l_solePose(0.0, 0.0, 0.004, 0.0, -1.0, 0.0, 0.0);
    gazeboCurrentStateDisp = gazeboOffset * l_solePose * currentState.getPosition();
    gazeboDesiredStateDisp = gazeboOffset * l_solePose * desiredState.getPosition();

    frameOutputBottle.addDouble(gazeboCurrentStateDisp.x());
    frameOutputBottle.addDouble(gazeboCurrentStateDisp.y());
    frameOutputBottle.addDouble(gazeboCurrentStateDisp.z());
    frameOutputBottle.addDouble(gazeboCurrentStateDisp.qw());
    frameOutputBottle.addDouble(gazeboCurrentStateDisp.qx());
    frameOutputBottle.addDouble(gazeboCurrentStateDisp.qy());
    frameOutputBottle.addDouble(gazeboCurrentStateDisp.qz());

    targetOutputBottle.addDouble(gazeboDesiredStateDisp.x());
    targetOutputBottle.addDouble(gazeboDesiredStateDisp.y());
    targetOutputBottle.addDouble(gazeboDesiredStateDisp.z());
    targetOutputBottle.addDouble(gazeboDesiredStateDisp.qw());
    targetOutputBottle.addDouble(gazeboDesiredStateDisp.qx());
    targetOutputBottle.addDouble(gazeboDesiredStateDisp.qy());
    targetOutputBottle.addDouble(gazeboDesiredStateDisp.qz());

    framePortOut.write(frameOutputBottle);
    targetPortOut.write(targetOutputBottle);

}

void TaskWidget::refreshParameters()
{
    m_weight = taskCon->getWeight()[0];
//    m_level = taskCon->getLevel();
    m_kp = taskCon->getStiffness();
    m_kd = taskCon->getDamping();
    emit parameterValuesChanged();
    updateParameterValues();

}

void TaskWidget::on_weightLineEdit_editingFinished()
{
    bool ok;
    double weight = ui->weightLineEdit->text().toDouble(&ok);
    if (ok) {
        taskCon->setWeight(weight);
        m_weight = taskCon->getWeight()[0];
    }
    ui->weightLineEdit->setText(QString::number(m_weight));
}

void TaskWidget::on_levelLineEdit_editingFinished()
{

}

void TaskWidget::on_kpLineEdit_editingFinished()
{
    bool ok;
    double kp = ui->kpLineEdit->text().toDouble(&ok);
    if (ok) {
        taskCon->setStiffness(kp);
        taskCon->setDamping(2.0*std::sqrt(kp));
        m_kp = taskCon->getStiffness();
        m_kd = taskCon->getDamping();
    }
    ui->kpLineEdit->setText(QString::number(m_kp));
    ui->kdLineEdit->setText(QString::number(m_kd));
}

void TaskWidget::on_kdLineEdit_editingFinished()
{
    bool ok;
    double kd = ui->kdLineEdit->text().toDouble(&ok);
    if (ok) {
        taskCon->setDamping(kd);
        m_kd = taskCon->getDamping();
    }
    ui->kdLineEdit->setText(QString::number(m_kd));
}

void TaskWidget::readCurrentStatePort()
{
    currentStateBottle = taskStateInPort.read(false);
    if (currentStateBottle!=NULL) {
        int dummyInt;
        currentState.extractFromBottle(*currentStateBottle, dummyInt);
        currentStateBottle->clear();
        emit currentStateUpdated();
    }
}

void TaskWidget::readDesiredStatePort()
{
    desiredStateBottle = taskDesiredStateInPort.read(false);
    if (desiredStateBottle!=NULL) {
        int dummyInt;
        desiredState.extractFromBottle(*desiredStateBottle, dummyInt);
        desiredStateBottle->clear();
        emit desiredStateUpdated();
    }
}

void TaskWidget::changeGazeboOffset(const Eigen::Displacementd& newOffset)
{
    gazeboOffset = newOffset;
}
