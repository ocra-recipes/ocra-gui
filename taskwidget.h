#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>
#include <QTimer>
#include <ocra-recipes/TaskConnection.h>
#include <ocra/control/TaskState.h>
#include <ocra/util/StringUtilities.h>
#include <yarp/os/all.h>
#include "messagetypes.h"
#include "yarptools.h"
#include "physicstools.h"


namespace Ui {
class TaskWidget;
}

class TaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWidget(const std::string& name, QWidget *parent = 0);
    virtual ~TaskWidget();

    void refreshParameters();

    QString poseToString(const std::vector<double> &poseVector);
    bool connectToGazebo();
    QString displacementToString(const Eigen::Displacementd &disp);
public slots:
    void updateCurrentState();
    void updateDesiredState();

    void connectPorts();
    void disconnectPorts();

    void disconnectDesiredStateInput();
    void sendAndReconnectDesiredStateInput();
    void changeGazeboOffset(const Eigen::Displacementd &newOffset);
private slots:
    void on_activateButton_clicked(bool checked);

    void on_deactivateButton_clicked(bool checked);

    void on_gazeboButton_clicked(bool checked);

    void on_weightLineEdit_editingFinished();

    void on_levelLineEdit_editingFinished();

    void on_kpLineEdit_editingFinished();

    void on_kdLineEdit_editingFinished();

    void readCurrentStatePort();

    void readDesiredStatePort();

private:
    void updateParameterValues();
    void sendPosesToGazebo();

signals:
    void addGazeboFrames(const std::string& name);
    void removeGazeboFrames(const std::string& name);
    void parameterValuesChanged();
    void currentStateUpdated();
    void desiredStateUpdated();
    void showUserMessage(const std::string& msg, MESSAGE_TYPE type);
    void getRobotWorldPose();

private:
    Ui::TaskWidget *ui;
    ocra_recipes::TaskConnection::Ptr taskCon;

    bool currentlyEditing;
    std::string m_name;
    std::string m_type;
    double m_weight;
    int m_level;
    double m_kp;
    double m_kd;

private:
    yarp::os::Network yarp;

    std::string taskStatePortOutName;
    std::string taskStatePortInName;

    std::string framePortInName;
    std::string framePortOutName;

    std::string targetPortInName;
    std::string targetPortOutName;

    yarp::os::Port framePortOut;
    yarp::os::Port targetPortOut;
    yarp::os::BufferedPort<yarp::os::Bottle> taskStateInPort;
    yarp::os::Bottle* currentStateBottle;

    std::string taskDesiredStatePortOutName;
    std::string taskDesiredStatePortInName;
    yarp::os::BufferedPort<yarp::os::Bottle> taskDesiredStateInPort;
    yarp::os::Bottle* desiredStateBottle;

    ocra::TaskState currentState;

    ocra::TaskState desiredState;

    QTimer* m_currentStatePortReadTimer;
    QTimer* m_desiredStatePortReadTimer;

    yarp::os::Bottle frameOutputBottle;
    std::vector<double> framePose;
    yarp::os::Bottle targetOutputBottle;
    std::vector<double> targetPose;

    Eigen::Displacementd gazeboCurrentStateDisp;
    Eigen::Displacementd gazeboDesiredStateDisp;
    Eigen::Displacementd gazeboOffset;

};

#endif // TASKWIDGET_H
