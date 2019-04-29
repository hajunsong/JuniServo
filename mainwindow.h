#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QTimer>
#include <QThread>
#include "juniservo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void ReadSettings();
    void WriteSettings();

public slots:
    void btnReadyClicked();
    void btnRunClicked();
    void btnConnectClicked();
    void btnApplyClicked();

    void update();

private:
    Ui::MainWindow *ui;
    JuniServo *servo;
    QTimer *timer;
    bool connectState, direction, run;
    uint vel_new, tor_new;
};

#endif // MAINWINDOW_H
