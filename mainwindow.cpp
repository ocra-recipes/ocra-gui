#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, taskWidgetGridRows(10)
, taskWidgetGridCols(4)
, currentTaskGridRowIndex(0)
, currentTaskGridColIndex(0)
{
    ui->setupUi(this);
    m_controllerServerRpcName = "/ControllerServer/rpc:i";
    m_guiRpcClientName = "/OcraGui/Controller/rpc:o";
    m_port.open(m_guiRpcClientName);

    m_gazeboTools = new GazeboTools;
    connect(m_gazeboTools, SIGNAL(showUserMessage(std::string,MESSAGE_TYPE)), this, SLOT(showMessage(std::string,MESSAGE_TYPE)));

    m_taskButtons = new QButtonGroup();
    connect(m_taskButtons, SIGNAL(buttonClicked(int)), this, SLOT(toggleTaskWidget(int)));


    ui->connectionStatusLabel->setText("Controller Disconnected");

    m_conMon = new ConnectionMonitor(m_guiRpcClientName, m_controllerServerRpcName);

    connect(m_conMon, SIGNAL(connected()), this, SLOT(openClientCommunications()));
    connect(m_conMon, SIGNAL(disconnected()), this, SLOT(disconnectGui()));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_taskButtons;
    delete m_gazeboTools;
}


void MainWindow::showMessage(const std::__cxx11::string &msg, MESSAGE_TYPE type)
{
    switch (type) {
    case INFO:
        showInfo(msg);
        break;

    case WARNING:
        showWarning(msg);
        break;

    case ERROR:
        showError(msg);
        break;

    default:
        break;
    }
}


int MainWindow::buttonIndexToTaskIndex(int buttonIndex)
{
    return std::abs(buttonIndex)-2;
}

void MainWindow::toggleTaskWidget(int buttonIndex)
{
    int id = buttonIndexToTaskIndex(buttonIndex);
    std::string name = m_taskList[id];
    for (auto n : m_taskWidgetMap) {
        if (n.first == name) {
            removeTaskWidget(name);
            return;
        }
    }
    addTaskWidget(name);
}

void MainWindow::addTaskWidget(const std::string& name)
{
    TaskWidget* task = new TaskWidget(name);
    connect(task, SIGNAL(addGazeboFrames(std::string)), m_gazeboTools, SLOT(addTaskFrames(std::string)));
    connect(task, SIGNAL(removeGazeboFrames(std::string)), m_gazeboTools, SLOT(removeTaskFrames(std::string)));
    connect(task, SIGNAL(showUserMessage(std::string,MESSAGE_TYPE)), this, SLOT(showMessage(std::string,MESSAGE_TYPE)) );

//    connect(this, SIGNAL(controllerConnected()), task, SLOT(connectPorts()));
//    connect(this, SIGNAL(controllerDisconnected()), task, SLOT(disconnectPorts()));

    ui->taskWidgetLayout->addWidget(task, currentTaskGridRowIndex, currentTaskGridColIndex, 1, 1);
    ++currentTaskGridColIndex;
    if (currentTaskGridColIndex>=taskWidgetGridCols) {
        ++currentTaskGridRowIndex;
        currentTaskGridColIndex = 0;
    }
    m_taskWidgetMap[name] = task;
}

void MainWindow::removeTaskWidget(const std::string &name)
{
    //
}

void MainWindow::getTaskList()
{
    std::vector<std::string> taskNames = m_clientComs.getTaskNames();
    if (tasksHaveChanged(taskNames)) {
        clearTasks();
        populateTasks(taskNames);
    } else {
        reconnectTasks();
    }
}

bool MainWindow::tasksHaveChanged(const std::vector<std::string>& taskNames)
{
    if (m_taskList.size() != taskNames.size()) {
        return true;
    }
    for (int i = 0; i<m_taskList.size(); ++i) {
        if (m_taskList[i]!=taskNames[i]) {
            return true;
        }
    }
    return false;
}

void MainWindow::clearTasks()
{

}

void MainWindow::populateTasks(const std::vector<std::string>& taskNames)
{
    m_taskList = taskNames;
    m_taskTypes = m_clientComs.getTaskTypes();

    for (auto i=0; i<m_taskList.size(); ++i) {
        auto name = m_taskList[i];
        QPushButton* btn = new QPushButton(name.c_str());
        if (m_taskTypes[i]=="point_contact") {
            btn->setEnabled(false);
        }
        ui->taskButtonLayout->addWidget(btn);
        m_taskButtons->addButton(btn);
    }
}

void MainWindow::reconnectTasks()
{

}

void MainWindow::openClientCommunications()
{
    if( m_clientComs.open(1, true) ) {
        m_clientComRpcPortName = m_clientComs.getClientPortName();
        ui->connectionStatusLabel->setText("Controller Connected");
        getTaskList();
        showInfo("Client communications opened.");
        emit controllerConnected();
    } else {
        showInfo("No controller server running");
    }
}

void MainWindow::disconnectGui()
{
    ui->connectionStatusLabel->setText("Controller Disconnected");
    showInfo("No controller server running");
    emit controllerDisconnected();
}

void MainWindow::showInfo(const std::string& message)
{
    std::string msg = "INFO: " + message;
    ui->userInformation->append(msg.c_str());
}

void MainWindow::showWarning(const std::string& message)
{
    std::string msg = "WARNING: " + message;
    ui->userInformation->append(msg.c_str());
}

void MainWindow::showError(const std::string& message)
{
    std::string msg = "ERROR: " + message;
    ui->userInformation->append(msg.c_str());
}

void MainWindow::on_clearInfoButton_clicked()
{
    ui->userInformation->clear();
}
