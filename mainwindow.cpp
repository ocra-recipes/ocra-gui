#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    TaskWidget* task = new TaskWidget;
    ui->taskWidgetLayout->addWidget(task,0,0,1,1);

}

MainWindow::~MainWindow()
{
    delete ui;
}
