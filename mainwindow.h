#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QSettings>
#include <QtDebug>
#include <QCheckBox>

#include <iostream>
using namespace std;
typedef unsigned int uint;

#define REG_PRODUCT_NO 0x00         // Product ID
#define REG_PRODUCT_VER 0x02        // Product Version
#define REG_FIRMWARE_VER 0x04       // Product Firmware Version
#define REG_SERIAL_NO_SUB 0x06      // Product Serial Number Low 2 Byte
#define REG_SERIAL_NO_MAIN 0x08     // Product Serial Number High 2 Byte

#define REG_STATUS_FLAG 0x0A        // Current Servo Status
#define REG_POSITION 0x0C           // Current Position
#define REG_VELOCITY 0x0E           // Current Velocity
#define REG_TORQUE 0x10             // Current Torque
#define REG_VOLTAGE 0x12            // Current Supllied Voltage
#define REG_TEMPER 0x14             // Current Temperature

#define REG_POSITION_NEW 0x1E       // New Position Configuration
#define REG_VELOCITY_NEW 0x20       // New Velocity Configuration
#define REG_TORQUE_NEW 0x22         // New Torque Configuration
#define REG_TURN_NEW 0x24           // 360 deg Turn Configuration

#define REG_ID 0x32                 // ID Configuration
#define REG_BAUDRATE 0x34           // Baud Rate Configuration
#define REG_SIGNAL_MODE 0x36        // Signal Tx/Rx Configuration
#define REG_SIMPLE_RETURN_DELAY 0x38// Return Packet Transmit Delay Configuration after Receive High Pulse in Simple Mode
#define REG_NORMAL_RETURN_DELAY 0x3A// Return Packet Transmit Delay Configuration after Receive High Pulse in Normal Mode
#define REG_POWER_CONFIG 0x46       // Power Configuration
#define REG_EMERGENCY_STOP 0x48     // Emergency Stop Configuration(depend on STATUS_FLAG)
#define REG_ACTION_MODE 0x4A        // Configuration of Acceleration, Deceleration, Velocity, Time and so on
#define REG_POSITION_SLOPE 0x4C     // Position Slope Configuration
#define REG_POSITION_DEADBAND 0x4E  // Position Dead Band Configuration
#define REG_POSITION_MAX 0x50       // Maximum Position Configuration at CW
#define REG_POSITION_MIN 0x52       // Maximum Position Configuration at CCW
#define REG_VELOCITY_MAX 0x54       // Maximum Velocity Configuration
#define REG_TORQUE_MAX 0x56         // Maximum Torque Configuration
#define REG_VOLTAGE_MAX 0x58        // Maximum Voltage Configuration
#define REG_VOLTAGE_MIN 0x5A        // Minimum Voltage Configuration
#define REG_TEMPER_MAX 0x5C         // Maximum Temperature Configuration
#define REG_FACTORY_DEFAULT 0x6E    // Factory Reset
#define REG_CONFIG_SAVE 0x70        // Changed Configuration Register Save in Flash Memory
#define REG_CONFIG_LOCK 0x72        // Locking Change Configuration Register

enum STATUS_FLAG{
    SIGNAL_ADDRESS_ERROR=1,
    SIGNAL_CHECKSUM_ERROR=2,
    SIGNAL_INTERVAL_ERROR=4,
    SIGNAL_LIMIT_ERROR=8,
    SIGNAL_FORMAT_ERROR=16,
    SIGNAL_SEND_ERROR=32,
    POSITION_MIN_OVER=1,
    POSITION_MAX_OVER=2,
    TEMPER_OVER=4,
    TORQUE_OVER=8,
    VOLTAGE_MIN_OVER=16,
    VOLTAGE_MAX_OVER=32
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ReadSettings();    // Load up qt program settings at startup
    void WriteSettings();   // Save qt program settings when closing

private slots:
    void btnConnectClicked();
    void update();
    void updateProductInfor();
    void updateConfigInfor();

    void readData();
    void handleError(QSerialPort::SerialPortError error);

    void sliderValueChanged();

    void btnProductInforClicked();
    void btnConfigInforClicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial = nullptr;
    QTimer *timer, *timerProductInfor, *timerConfigInfor;
    uint indx_pd, indx_conf, indx;


    uint regProductInfor[3] = {REG_PRODUCT_NO, REG_PRODUCT_VER, REG_FIRMWARE_VER};
    uint regConfigInfor[10] = {REG_ID, REG_POSITION_SLOPE, REG_POSITION_DEADBAND, REG_POSITION_MAX, REG_POSITION_MIN,
                               REG_VELOCITY_MAX, REG_TORQUE_MAX, REG_VOLTAGE_MAX, REG_VOLTAGE_MIN, REG_TEMPER_MAX};
    uint regCurrenInfor[4] = {REG_POSITION, REG_VELOCITY, REG_TORQUE, REG_STATUS_FLAG};
};

#endif // MAINWINDOW_H
