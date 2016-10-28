#include "taskwidget.h"
#include "ui_taskwidget.h"

TaskWidget::TaskWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskWidget)
{
    ui->setupUi(this);
}

TaskWidget::~TaskWidget()
{
    delete ui;
}

void TaskWidget::on_activateButton_clicked(bool checked)
{

}

void TaskWidget::on_deactivateButton_clicked(bool checked)
{

}

void TaskWidget::on_gazeboButton_clicked(bool checked)
{

}
