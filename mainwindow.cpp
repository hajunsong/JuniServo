#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ReadSettings();

    serial = new QSerialPort(this);

    connect(ui->btnConnect, &QPushButton::clicked, this, &MainWindow::btnConnectClicked);

    connect(serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::update);
    timer->setInterval(15);

    indx = 0;
    indx_pd = 0;
    indx_conf = 0;

    ui->hsPos->setEnabled(false);
    ui->hsVel->setEnabled(false);
    ui->hsTor->setEnabled(false);

    connect(ui->hsPosNew, &QSlider::sliderPressed, timer, &QTimer::stop);
    connect(ui->hsTorNew, &QSlider::sliderPressed, timer, &QTimer::stop);
    connect(ui->hsVelNew, &QSlider::sliderPressed, timer, &QTimer::stop);
    connect(ui->hsPosNew, &QSlider::sliderMoved, timer, &QTimer::stop);
    connect(ui->hsTorNew, &QSlider::sliderMoved, timer, &QTimer::stop);
    connect(ui->hsVelNew, &QSlider::sliderMoved, timer, &QTimer::stop);
    connect(ui->hsPosNew, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
    connect(ui->hsTorNew, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
    connect(ui->hsVelNew, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
    connect(ui->hsPosNew, &QSlider::sliderReleased, this, &MainWindow::sliderValueChanged);
    connect(ui->hsTorNew, &QSlider::sliderReleased, this, &MainWindow::sliderValueChanged);
    connect(ui->hsVelNew, &QSlider::sliderReleased, this, &MainWindow::sliderValueChanged);

    connect(ui->btnPdInfor, &QPushButton::clicked, this, &MainWindow::btnProductInforClicked);
    connect(ui->btnConfInfor, &QPushButton::clicked, this, &MainWindow::btnConfigInforClicked);
    timerProductInfor = new QTimer(this);
    connect(timerProductInfor, &QTimer::timeout, this, &MainWindow::updateProductInfor);
    timerProductInfor->setInterval(15);
    timerConfigInfor = new QTimer(this);
    connect(timerConfigInfor, &QTimer::timeout, this, &MainWindow::updateConfigInfor);
    timerConfigInfor->setInterval(15);
}

MainWindow::~MainWindow()
{
    serial->close();
    WriteSettings();
    delete ui;
}

void MainWindow::ReadSettings()
{
    QSettings settings("JUNISERVO", "Test Project");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    QString Port = settings.value("Port").toString();
    QString Baud = settings.value("Baud").toString();
    ui->txtPort->setText(Port);
    ui->txtBaud->setText(Baud);
}

void MainWindow::WriteSettings()
{
    QSettings settings("JUNISERVO", "Test Project");
    settings.setValue("Port", ui->txtPort->text());
    settings.setValue("Baud", ui->txtBaud->text());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}

void MainWindow::btnConnectClicked(){
    QString name = ui->txtPort->text();
    qint32 baud = ui->txtBaud->text().toInt();
    QSerialPort::DataBits bit = QSerialPort::DataBits::Data8;
    QSerialPort::Parity parity = QSerialPort::Parity::NoParity;
    QSerialPort::StopBits stop = QSerialPort::StopBits::OneStop;
    QSerialPort::FlowControl flow = QSerialPort::FlowControl::NoFlowControl;

    if (serial->isOpen()){
        serial->close();
        ui->btnConnect->setText("Connect");
        ui->txtPort->setEnabled(true);
        ui->txtBaud->setEnabled(true);
        qDebug() << tr("Disconnected to %1").arg(name);
        timer->stop();
    }
    else{
        serial->setPortName(name);
        serial->setBaudRate(baud);
        serial->setDataBits(bit);
        serial->setParity(parity);
        serial->setStopBits(stop);
        serial->setFlowControl(flow);
        if (serial->open(QIODevice::ReadWrite)){
            qDebug() << tr("Connected to %1 : %2, %3, %4, %5, %6")
                        .arg(name).arg(QString::number(baud)).arg(QString::number(bit))
                        .arg(QString::number(parity)).arg(QString::number(stop)).arg(QString::number(flow));
            ui->btnConnect->setText("Disconnect");
            ui->txtPort->setEnabled(false);
            ui->txtBaud->setEnabled(false);
            timer->start();
        }
        else{
            qDebug() << tr("Error %1").arg(serial->errorString());
        }
    }
}

void MainWindow::update(){
    uint head=0x96, id=0x01, addr = regCurrenInfor[indx],len=0, param=0, checksum=0;
    QByteArray data;
    checksum = (id + addr + len + param)%256;
    data.append(static_cast<char>(head));
    data.append(static_cast<char>(id));
    data.append(static_cast<char>(addr));
    data.append(static_cast<char>(len));
    data.append(static_cast<char>(checksum));
    qDebug() << "Tx data : " + data.toHex();
    serial->write(data);
    serial->sendBreak(1);

//    uint head=0x96, id=0x01, addr = 0x34, len=2, param = 5, checksum=0;
//    QByteArray data;
//    checksum = (id + addr + len + param)%256;
//    data.append(static_cast<char>(head));
//    data.append(static_cast<char>(id));
//    data.append(static_cast<char>(addr));
//    data.append(static_cast<char>(len));
//    data.append(static_cast<char>(param%256));
//    data.append(static_cast<char>(param/256));
//    data.append(static_cast<char>(checksum));
//    qDebug() << "Tx data : " + data.toHex();
//    serial->write(data);
//    serial->waitForBytesWritten(-1);

//    head = 0x96;
//    id = 0x01;
//    addr = 0x70;
//    len = 2;
//    checksum = (id + addr + len + 0xFF + 0xFF)%256;
//    data.append(static_cast<char>(head));
//    data.append(static_cast<char>(id));
//    data.append(static_cast<char>(addr));
//    data.append(static_cast<char>(len));
//    data.append(static_cast<char>(0xFF));
//    data.append(static_cast<char>(0xFF));
//    data.append(static_cast<char>(checksum));
//    qDebug() << "Tx data : " + data.toHex();
//    serial->write(data);
//    serial->waitForBytesWritten(-1);
}

void MainWindow::readData(){
    const QByteArray data = serial->readAll();
    qDebug() << "Rx data : " + data.toHex();
    uchar *ch = new uchar[data.length()];
    uint startIndex = 0;
    for (int j = 0; j < data.length(); j++) {
        ch[j] = static_cast<uchar>(data.at(j));
        if (ch[j] == 0x69){
            startIndex = static_cast<uint>(j);
        }
    }
    uint addr = ch[startIndex], id = ch[startIndex + 1];
    if (addr == 0x69) { // Header
//        timer->stop();
        if (id == 0x01){ // ID
            uint len = ch[startIndex + 3];
            uint bit[2] = {0,0};
            for(uint i = 0; i < len; i++){
                bit[i] = ch[startIndex + i + 4];
            }
            QString status_message = "";

            switch(ch[startIndex + 2]) // Address
            {
            case REG_PRODUCT_NO:
            {
                status_message += "Product Number : " + QString::number(bit[0]);
                ui->txtProdNo->setText(QString::number(bit[0]));
                break;
            }
            case REG_PRODUCT_VER:
            {
                status_message += "Product Version : " + QString::number(bit[0]);
                ui->txtProdVer->setText(QString::number(bit[0]));
                break;
            }
            case REG_FIRMWARE_VER:
            {
                status_message += "Firmware Version : " + QString::number(bit[0]);
                ui->txtFirmVer->setText(QString::number(bit[0]));
                break;
            }
            case REG_SERIAL_NO_SUB: {
                status_message += "Serial Number Low Byte : " + QString::number(bit[0]);
                break;
            }
            case REG_SERIAL_NO_MAIN: {
                status_message += "Serial Number High Byte : " + QString::number(bit[0]);
                break;
            }
            case REG_STATUS_FLAG:
            {
                if (bit[0] & STATUS_FLAG::SIGNAL_ADDRESS_ERROR) {
                    status_message += "Signal Address Error\n";
                    ui->cbSignalAddressError->setChecked(true);
                }
                else if(bit[0] & STATUS_FLAG::SIGNAL_CHECKSUM_ERROR) {
                    status_message += "Signal Checksum Error\n";
                    ui->cbSignalChecksumError->setChecked(true);
                }
                else if(bit[0] & STATUS_FLAG::SIGNAL_INTERVAL_ERROR){
                    status_message += "Signal Interval Error\n";
                    ui->cbSignalIntervalError->setChecked(true);
                }
                else if(bit[0] & STATUS_FLAG::SIGNAL_LIMIT_ERROR) {
                    status_message += "Signal Limit Error\n";
                    ui->cbSignalLimitError->setChecked(true);
                }
                else if(bit[0] & STATUS_FLAG::SIGNAL_FORMAT_ERROR) {
                    status_message += "Signal Format Error\n";
                    ui->cbSignalFormatError->setChecked(true);
                }
                else if(bit[0] & STATUS_FLAG::SIGNAL_SEND_ERROR) {
                    status_message += "Signal Send Error\n";
                    ui->cbSignalSendError->setChecked(true);
                }
                else if(bit[1] & STATUS_FLAG::POSITION_MIN_OVER) {
                    status_message += "Minimum Position Over\n";
                    ui->cbPosMinOver->setChecked(true);
                }
                else if(bit[1] & STATUS_FLAG::POSITION_MAX_OVER) {
                    status_message += "Maximum Position Over\n";
                    ui->cbPosMaxOver->setChecked(true);
                }
                else if(bit[1] & STATUS_FLAG::TEMPER_OVER) {
                    status_message += "Temperature Limit Over\n";
                    ui->cbTempOver->setChecked(true);
                }
                else if(bit[1] & STATUS_FLAG::TORQUE_OVER) {
                    status_message += "Torque Limit Over\n";
                    ui->cbTorOver->setChecked(true);
                }
                else if(bit[1] & STATUS_FLAG::VOLTAGE_MIN_OVER) {
                    status_message += "Minimum Voltage Over\n";
                    ui->cbVolMinOver->setChecked(true);
                }
                else if(bit[1] & STATUS_FLAG::VOLTAGE_MAX_OVER) {
                    status_message += "Maximum Voltage Over\n";
                    ui->cbVolMaxOver->setChecked(true);
                }
                else {
                    status_message += "Status No Error";
                }
                ui->cbSignalSendError->setEnabled(false);
                ui->cbSignalLimitError->setEnabled(false);
                ui->cbSignalFormatError->setEnabled(false);
                ui->cbSignalAddressError->setEnabled(false);
                ui->cbSignalChecksumError->setEnabled(false);
                ui->cbSignalIntervalError->setEnabled(false);
                ui->cbPosMaxOver->setEnabled(false);
                ui->cbPosMinOver->setEnabled(false);
                ui->cbVolMaxOver->setEnabled(false);
                ui->cbVolMinOver->setEnabled(false);
                ui->cbTorOver->setEnabled(false);
                ui->cbTempOver->setEnabled(false);
                break;
            }
            case REG_POSITION: {
                uint value = bit[1]*256 + bit[0];
                status_message += "Position : " + QString::number(value);
                ui->hsPos->setValue(static_cast<int>(value));
                ui->txtPos->setText(QString::number(value));
                break;
            }
            case REG_VELOCITY:{
                int value = static_cast<char>(bit[1])*256 + static_cast<char>(bit[0]);
                status_message += "Velocity : " + QString::number(value);
                ui->hsVel->setValue(value < 0 ? -value : value);
                ui->txtVel->setText(QString::number(value));
                break;
            }
            case REG_TORQUE:{
                int value = static_cast<char>(bit[1])*256 + static_cast<char>(bit[0]);
                status_message += "Torque : " + QString::number(value);
                ui->hsTor->setValue(value < 0 ? -value : value);
                ui->txtTor->setText(QString::number(value));
                break;
            }
            case REG_VOLTAGE:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Voltage : " + QString::number(value);
                break;
            }
            case REG_TEMPER: {
                uint value = bit[1]*256 + bit[0];
                status_message += "Temperature : " + QString::number(value);
                break;
            }
            case REG_POSITION_NEW:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Position New : " + QString::number(value);
//                ui->hsPosNew->setValue(value);
                ui->txtPosNew->setText(QString::number(value));
                break;
            }
            case REG_VELOCITY_NEW:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Velocity New : " + QString::number(value);
//                ui->hsVelNew->setValue(value);
                ui->txtVelNew->setText(QString::number(value));
                break;
            }
            case REG_TORQUE_NEW:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Torque New : " + QString::number(value);
//                ui->hsTorNew->setValue(value);
                ui->txtTorNew->setText(QString::number(value));
                break;
            }
            case REG_TURN_NEW:{
                break;
            }
            case REG_ID:
            {
                status_message += "ID : " + QString::number(*bit);
                ui->txtID->setText(QString::number(*bit));
                break;
            }
            case REG_BAUDRATE:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Baud Rate : " + QString::number(value);
                break;
            }
            case REG_SIGNAL_MODE:{
                break;
            }
            case REG_SIMPLE_RETURN_DELAY:{
                break;
            }
            case REG_NORMAL_RETURN_DELAY:{
                break;
            }
            case REG_POWER_CONFIG:{
                break;
            }
            case REG_EMERGENCY_STOP:{
                break;
            }
            case REG_ACTION_MODE:{
                break;
            }
            case REG_POSITION_SLOPE:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Position Slope : " + QString::number(value);
                ui->txtPosSlope->setText(QString::number(value));
                break;
            }
            case REG_POSITION_DEADBAND:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Position Deadband : " + QString::number(value);
                ui->txtPosDead->setText(QString::number(value));
                break;
            }
            case REG_POSITION_MAX:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Position Max : " + QString::number(value);
                ui->txtPosMax->setText(QString::number(value));
                ui->hsPos->setMaximum(static_cast<int>(value));
                ui->hsPosNew->setMaximum(static_cast<int>(value));
                break;
            }
            case REG_POSITION_MIN:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Position Min : " + QString::number(value);
                ui->txtPosMin->setText(QString::number(value));
                ui->hsPos->setMinimum(static_cast<int>(value));
                ui->hsPosNew->setMinimum(static_cast<int>(value));
                break;
            }
            case REG_VELOCITY_MAX:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Velocity Max : " + QString::number(value);
                ui->txtVelMax->setText(QString::number(value));
                ui->hsVel->setRange(0, static_cast<int>(value));
                ui->hsVelNew->setRange(0, static_cast<int>(value));
                break;
            }
            case REG_TORQUE_MAX:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Torque Max : " + QString::number(value);
                ui->txtTorMax->setText(QString::number(value));
                ui->hsTor->setRange(0, static_cast<int>(value));
                ui->hsTorNew->setRange(0, static_cast<int>(value));
                break;
            }
            case REG_VOLTAGE_MAX:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Voltage Max : " + QString::number(value);
                ui->txtVolMax->setText(QString::number(value));
                break;
            }
            case REG_VOLTAGE_MIN:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Voltage Min : " + QString::number(value);
                ui->txtVolMin->setText(QString::number(value));
                break;
            }
            case REG_TEMPER_MAX:{
                uint value = bit[1]*256 + bit[0];
                status_message += "Temperature Max : " + QString::number(value);
                ui->txtTempMax->setText(QString::number(value));
                break;
            }
            default: {
                status_message += "Nothing";
                break;
            }
            }
//            qDebug() << status_message;
            delete[] ch;
        }
        ui->status->setText(QString::number(indx));
        indx = indx >= 4 ? 0 : indx + 1;
        indx_pd++;
        indx_conf++;
        if (indx_pd >= 3){
            timerProductInfor->stop();
            if (ui->txtProdNo->text().length() > 0 &&
                    ui->txtProdVer->text().length() > 0 &&
                    ui->txtFirmVer->text().length() > 0)
            {
                timer->start();
            }
            else{
                timerProductInfor->start();
                indx_pd = 0;
            }
        }
        if (indx_conf >= 10){
            timerConfigInfor->stop();
            if (ui->txtID->text().length() > 0 &&
                    ui->txtPosSlope->text().length() > 0 &&
                    ui->txtPosDead->text().length() > 0 &&
                    ui->txtPosMax->text().length() > 0 &&
                    ui->txtPosMin->text().length() > 0 &&
                    ui->txtVelMax->text().length() > 0 &&
                    ui->txtTorMax->text().length() > 0 &&
                    ui->txtVolMax->text().length() > 0 &&
                    ui->txtVolMin->text().length() > 0 &&
                    ui->txtTempMax->text().length() > 0)
            {
                timer->start();
            }
            else{
                timerConfigInfor->start();
                indx_conf = 0;
            }

        }
//        timer->start();
    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error){
    if (error == QSerialPort::ResourceError) {
        qDebug() << tr("Critical Error %1").arg(serial->errorString());
        serial->close();
    }
}

void MainWindow::sliderValueChanged()
{
    QString objName = sender()->objectName();
    int value = 0;
    int head=0x96, id=0x01, addr = 0, len=2, paramH, paramL, checksum=0;
    if (objName.contains("Pos")){
        addr = REG_POSITION_NEW;
        value = ui->hsPosNew->value();
        ui->txtPosNew->setText(QString::number(value));
    }
    else if(objName.contains("Vel")){
        addr = REG_VELOCITY_NEW;
        value = ui->hsVelNew->value();
        ui->txtVelNew->setText(QString::number(value));
    }
    else if(objName.contains("Tor")){
        addr = REG_TORQUE_NEW;
        value = ui->hsTorNew->value();
        ui->txtTorNew->setText(QString::number(value));
    }
    paramH = value/256;
    paramL = value%256;
    QByteArray data;
    checksum = (id + addr + len + paramH + paramL)%256;
    data.append(static_cast<char>(head));
    data.append(static_cast<char>(id));
    data.append(static_cast<char>(addr));
    data.append(static_cast<char>(len));
    data.append(static_cast<char>(paramL));
    data.append(static_cast<char>(paramH));
    data.append(static_cast<char>(checksum));

//    qDebug() << "Tx data : " + data.toHex();
    serial->write(data);
//    serial->sendBreak(1);
    timer->start();
}

void MainWindow::btnProductInforClicked(){
    timer->stop();
    indx_pd = 0;
    timerProductInfor->start();
}

void MainWindow::btnConfigInforClicked(){
    timer->stop();
    indx_conf = 0;
    timerConfigInfor->start();
}

void MainWindow::updateProductInfor(){
    uint head=0x96, id=0x01, addr = regProductInfor[indx_pd],len=0, param=0, checksum=0;
    QByteArray data;
    checksum = (id + addr + len + param)%256;
    data.append(static_cast<char>(head));
    data.append(static_cast<char>(id));
    data.append(static_cast<char>(addr));
    data.append(static_cast<char>(len));
    data.append(static_cast<char>(checksum));
    qDebug() << "Tx data : " + data.toHex();
    serial->write(data);
    serial->sendBreak(1);
}

void MainWindow::updateConfigInfor(){
    uint head=0x96, id=0x01, addr = regConfigInfor[indx_conf],len=0, param=0, checksum=0;
    QByteArray data;
    checksum = (id + addr + len + param)%256;
    data.append(static_cast<char>(head));
    data.append(static_cast<char>(id));
    data.append(static_cast<char>(addr));
    data.append(static_cast<char>(len));
    data.append(static_cast<char>(checksum));
    qDebug() << "Tx data : " + data.toHex();
    serial->write(data);
    serial->sendBreak(1);
}
