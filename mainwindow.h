#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QTimer>
#include <QPushButton>
#include <ocra-recipes/ClientCommunications.h>
#include <yarp/os/all.h>
#include "gazebotools.h"
#include "taskwidget.h"
#include "messagetypes.h"
#include "yarptools.h"
#include "connectionmonitor.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void showInfo(const std::string &message);

    int buttonIndexToTaskIndex(int buttonIndex);
    void addTaskWidget(const std::string &name);
    void removeTaskWidget(const std::string &name);
    void showWarning(const std::string &message);
    void showError(const std::string &message);
private:
    bool tasksHaveChanged(const std::vector<std::string> &taskNames);
    void clearTasks();
    void populateTasks(const std::vector<std::string> &taskNames);
    void reconnectTasks();

public slots:
    void toggleTaskWidget(int buttonIndex);
    void showMessage(const std::string& msg, MESSAGE_TYPE type = MESSAGE_TYPE::INFO);

    void openClientCommunications();
    void getTaskList();
    void disconnectGui();
signals:
    void controllerConnected();
    void controllerDisconnected();

private slots:
    void on_clearInfoButton_clicked();

private:
    Ui::MainWindow *ui;

    QButtonGroup *m_taskButtons;

    GazeboTools *m_gazeboTools;

    ConnectionMonitor *m_conMon;

    int taskWidgetGridRows;
    int taskWidgetGridCols;
    int currentTaskGridRowIndex;
    int currentTaskGridColIndex;

    yarp::os::Network yarp;
    yarp::os::RpcClient m_port;

    ocra_recipes::ClientCommunications m_clientComs;
    std::string m_clientComRpcPortName;

    std::vector<std::string> m_taskList;
    std::vector<std::string> m_taskTypes;
    std::vector<std::string> m_currentTaskWidgetList;

    std::string m_controllerServerRpcName;
    std::string m_guiRpcClientName;


};

#endif // MAINWINDOW_H
