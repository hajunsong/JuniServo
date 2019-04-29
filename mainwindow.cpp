#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QList<QString> portItems = {"COM1","COM2","COM3","COM4","COM5"};
    QList<QString> baudItems = {"9600","14400","19200","38400","57600","115200","229800","459700","930200"};
    ui->comboPort->addItems(portItems);
    ui->comboBaud->addItems(baudItems);

    ReadSettings();

    servo = new JuniServo();

    connect(ui->btnConnect, &QPushButton::clicked, this, &MainWindow::btnConnectClicked);
    connect(ui->btnReady, &QPushButton::clicked, this, &MainWindow::btnReadyClicked);
    connect(ui->btnRun, &QPushButton::clicked, this, &MainWindow::btnRunClicked);

    connectState = false;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update);
    timer->setInterval(100);
    direction = true;
    run = false;

    vel_new = 0;
    tor_new = 0;
    connect(ui->btnApply, &QPushButton::clicked, this, &MainWindow::btnApplyClicked);
}

MainWindow::~MainWindow()
{
    WriteSettings();
    delete servo;
    delete ui;
}

void MainWindow::ReadSettings()
{
    QSettings settings("JUNISERVO", "MultiTest");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    QString Velocity = settings.value("Velocity").toString();
    QString Torque = settings.value("Torque").toString();
    int indexPort = settings.value("Port").toInt();
    int indexBaud = settings.value("Baud").toInt();
    ui->txtVelNew->setText(Velocity);
    ui->txtTorNew->setText(Torque);
    ui->comboPort->setCurrentIndex(indexPort);
    ui->comboBaud->setCurrentIndex(indexBaud);
}

void MainWindow::WriteSettings()
{
    QSettings settings("JUNISERVO", "MultiTest");
    settings.setValue("Velocity", ui->txtVelNew->text());
    settings.setValue("Torque", ui->txtTorNew->text());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("Port",ui->comboPort->currentIndex());
    settings.setValue("Baud",ui->comboBaud->currentIndex());
}

void MainWindow::btnConnectClicked(){
    if (connectState){
        servo->disconnect();
        connectState = false;
        ui->btnConnect->setText("Connect");
    }
    else{
        connectState = servo->connect(ui->comboPort->currentText(), ui->comboBaud->currentText());
        ui->btnConnect->setText("Disconnect");
    }
}

void MainWindow::btnReadyClicked(){
    for(uint i = 1; i <= 2; i++){
        QString prod_no = QString::number(servo->readProdNo(i));
        QString prod_ver = QString::number(servo->readProdVer(i));
        QString firm_ver = QString::number(servo->readFirmVer(i));
        QString pos_slope = QString::number(servo->readPositionSlope(i));
        QString pos_max = QString::number(servo->readPositionMax(i));
        QString pos_min = QString::number(servo->readPositionMin(i));
        QString vel_max = QString::number(servo->readVelocityMax(i));
        QString tor_max = QString::number(servo->readTorqueMax(i));
        QString vol_max = QString::number(servo->readVoltageMax(i));
        QString vol_min = QString::number(servo->readVoltageMin(i));
        QString tem_max = QString::number(servo->readTemperMax(i));
        uint pos = servo->readPosition(i);
        int vel = servo->readVelocity(i);
        int tor = servo->readTorque(i);
        if (i == 1){
            ui->txtProdNo->setText(prod_no);
            ui->txtProdVer->setText(prod_ver);
            ui->txtFirmVer->setText(firm_ver);
            ui->txtPosSlope->setText(pos_slope);
            ui->txtPosMax->setText(pos_max);
            ui->txtPosMin->setText(pos_min);
            ui->txtVelMax->setText(vel_max);
            ui->txtTorMax->setText(tor_max);
            ui->txtVolMax->setText(vol_max);
            ui->txtVolMin->setText(vol_min);
            ui->txtTempMax->setText(tem_max);

            ui->txtPos->setText(QString::number(pos));
            ui->txtVel->setText(QString::number(vel));
            ui->txtTor->setText(QString::number(tor));

            ui->vsPos->setRange(pos_min.toInt(), pos_max.toInt());
            ui->vsVel->setRange(0, vel_max.toInt());
            ui->vsTor->setRange(0, tor_max.toInt());
            ui->vsPos->setValue(static_cast<int>(pos));
            ui->vsVel->setValue(vel);
            ui->vsTor->setValue(tor);
        }
        else if(i == 2){
            ui->txtProdNo_2->setText(prod_no);
            ui->txtProdVer_2->setText(prod_ver);
            ui->txtFirmVer_2->setText(firm_ver);
            ui->txtPosSlope_2->setText(pos_slope);
            ui->txtPosMax_2->setText(pos_max);
            ui->txtPosMin_2->setText(pos_min);
            ui->txtVelMax_2->setText(vel_max);
            ui->txtTorMax_2->setText(tor_max);
            ui->txtVolMax_2->setText(vol_max);
            ui->txtVolMin_2->setText(vol_min);
            ui->txtTempMax_2->setText(tem_max);

            ui->txtPos_2->setText(QString::number(pos));
            ui->txtVel_2->setText(QString::number(vel));
            ui->txtTor_2->setText(QString::number(tor));

            ui->vsPos_2->setRange(pos_min.toInt(), pos_max.toInt());
            ui->vsVel_2->setRange(0, vel_max.toInt());
            ui->vsTor_2->setRange(0, tor_max.toInt());
            ui->vsPos_2->setValue(static_cast<int>(pos));
            ui->vsVel_2->setValue(vel);
            ui->vsTor_2->setValue(tor);
        }
        else if(i == 3){
            ui->txtProdNo_3->setText(prod_no);
            ui->txtProdVer_3->setText(prod_ver);
            ui->txtFirmVer_3->setText(firm_ver);
            ui->txtPosSlope_3->setText(pos_slope);
            ui->txtPosMax_3->setText(pos_max);
            ui->txtPosMin_3->setText(pos_min);
            ui->txtVelMax_3->setText(vel_max);
            ui->txtTorMax_3->setText(tor_max);
            ui->txtVolMax_3->setText(vol_max);
            ui->txtVolMin_3->setText(vol_min);
            ui->txtTempMax_3->setText(tem_max);

            ui->txtPos_3->setText(QString::number(pos));
            ui->txtVel_3->setText(QString::number(vel));
            ui->txtTor_3->setText(QString::number(tor));

            ui->vsPos_3->setRange(pos_min.toInt(), pos_max.toInt());
            ui->vsVel_3->setRange(0, vel_max.toInt());
            ui->vsTor_3->setRange(0, tor_max.toInt());
            ui->vsPos_3->setValue(static_cast<int>(pos));
            ui->vsVel_3->setValue(vel);
            ui->vsTor_3->setValue(tor);
        }
    }
}

void MainWindow::btnApplyClicked(){
    vel_new = ui->txtVelNew->text().toUInt();
    tor_new = ui->txtTorNew->text().toUInt();
    servo->writeGroupNewVelocity(static_cast<uint>(vel_new));
    servo->writeGroupNewTorque(static_cast<uint>(tor_new));
}

void MainWindow::btnRunClicked(){
    if (run){
        timer->stop();
        run = false;
        ui->btnRun->setText("Run");
    }
    else{
        timer->start();
        run = true;
        ui->btnRun->setText("Stop");
    }
}

void MainWindow::update(){
    uint pos1 = servo->readPosition(1);
    int vel1 = servo->readVelocity(1);
    int tor1 = servo->readTorque(1);
    uint pos2 = servo->readPosition(2);
    int vel2 = servo->readVelocity(2);
    int tor2 = servo->readTorque(2);
    uint pos3 = servo->readPosition(3);
    int vel3 = servo->readVelocity(3);
    int tor3 = servo->readTorque(3);

    if (vel1 < 0){

    }
    if (tor1 < 0){

    }
    if (vel2 < 0){

    }
    if (tor2 < 0){

    }
    if (vel3 < 0){

    }
    if (tor3 < 0){

    }

    ui->txtPos->setText(QString::number(pos1));
    ui->txtVel->setText(QString::number(vel1 > 0 ? vel1 : -vel1));
    ui->txtTor->setText(QString::number(tor1 > 0 ? tor1 : -tor1));

    ui->vsPos->setValue(static_cast<int>(pos1));
    ui->vsVel->setValue(vel1);
    ui->vsTor->setValue(tor1);

    ui->txtPos_2->setText(QString::number(pos2));
    ui->txtVel_2->setText(QString::number(vel2 > 0 ? vel2 : -vel2));
    ui->txtTor_2->setText(QString::number(tor2 > 0 ? tor2 : -tor2));

    ui->vsPos_2->setValue(static_cast<int>(pos2));
    ui->vsVel_2->setValue(vel2);
    ui->vsTor_2->setValue(tor2);

    ui->txtPos_3->setText(QString::number(pos3));
    ui->txtVel_3->setText(QString::number(vel3 > 0 ? vel3 : -vel3));
    ui->txtTor_3->setText(QString::number(tor3 > 0 ? tor3 : -tor3));

    ui->vsPos_3->setValue(static_cast<int>(pos3));
    ui->vsVel_3->setValue(vel3);
    ui->vsTor_3->setValue(tor3);

    uint value;
    if (direction){
        if (abs(static_cast<int>(pos1) - 1024) < 5 && abs(static_cast<int>(pos2) - 1024) < 5 && abs(static_cast<int>(pos3) - 1024) < 5){
            direction = false;
            value = 3072;
        }
        else{
            value = 1024;
        }
    }
    else{
        if (abs(static_cast<int>(pos1) - 3072) < 5 && abs(static_cast<int>(pos2) - 3072) < 5 && abs(static_cast<int>(pos3) - 3072) < 5){
            direction = true;
            value = 1024;
        }
        else{
            value = 3072;
        }
    }

    servo->writeGroupNewPosition(value);
}
