#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>

namespace Ui {
class TaskWidget;
}

class TaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWidget(QWidget *parent = 0);
    ~TaskWidget();

private slots:
    void on_activateButton_clicked(bool checked);

    void on_deactivateButton_clicked(bool checked);

    void on_gazeboButton_clicked(bool checked);

private:
    Ui::TaskWidget *ui;
};

#endif // TASKWIDGET_H
