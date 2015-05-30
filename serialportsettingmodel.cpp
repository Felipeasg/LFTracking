#include "serialportsettingmodel.h"

SerialPortSettingModel* SerialPortSettingModel::uniqueInstance = NULL;

SerialPortSettingModel::SerialPortSettingModel(QObject *parent) : QObject(parent)
{

}

SerialPortSettingModel::~SerialPortSettingModel()
{

}

SerialPortSettingModel *SerialPortSettingModel::getInstance()
{
    if(!uniqueInstance)
    {
        uniqueInstance = new SerialPortSettingModel();
    }

    return uniqueInstance;
}
int SerialPortSettingModel::getBaudRate() const
{
    return serialPortConfigSet.baudRate;
}

void SerialPortSettingModel::setBaudRate(int value)
{
    serialPortConfigSet.baudRate = value;
}
int SerialPortSettingModel::getDataBits() const
{
    return serialPortConfigSet.dataBits;
}

void SerialPortSettingModel::setDataBits(int value)
{
    serialPortConfigSet.dataBits = value;
}
int SerialPortSettingModel::getParity() const
{
    return serialPortConfigSet.parity;
}

void SerialPortSettingModel::setParity(int value)
{
    serialPortConfigSet.parity = value;
}
int SerialPortSettingModel::getStopBits() const
{
    return serialPortConfigSet.stopBits;
}

void SerialPortSettingModel::setStopBits(int value)
{
    serialPortConfigSet.stopBits = value;
}
SerialPortSettingsConfigset SerialPortSettingModel::getSerialPortConfigSet() const
{
    return serialPortConfigSet;
}

void SerialPortSettingModel::setSerialPortConfigSet(const SerialPortSettingsConfigset &value)
{
    serialPortConfigSet = value;
}
QString SerialPortSettingModel::getPortName() const
{
    return serialPortConfigSet.portName;
}

void SerialPortSettingModel::setPortName(const QString &value)
{
    serialPortConfigSet.portName = value;
}







