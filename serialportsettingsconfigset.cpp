#include "serialportsettingsconfigset.h"

SerialPortSettingsConfigset::SerialPortSettingsConfigset()
{
//    portName = "ttyUSB0";
    portName = "COM6";
    baudRate = 115200;
    dataBits = QSerialPort::Data8;
    parity = QSerialPort::NoParity;
    stopBits = QSerialPort::OneStop;
    flowControl = QSerialPort::NoFlowControl;
}

SerialPortSettingsConfigset::~SerialPortSettingsConfigset()
{

}

void SerialPortSettingsConfigset::insertToDataStream(QDataStream &dataStream) const
{

    dataStream << portName;
    dataStream << baudRate;
    dataStream << dataBits;
    dataStream << parity;
    dataStream << stopBits;
    dataStream << flowControl;
}

void SerialPortSettingsConfigset::extractFromDataStream(QDataStream &dataStream)
{
    dataStream >> portName;
    dataStream >> baudRate;
    dataStream >> dataBits;
    dataStream >> parity;
    dataStream >> stopBits;
    dataStream >> flowControl;
}

QDataStream &operator<<(QDataStream &dataStream, const SerialPortSettingsConfigset &object)
{
    object.insertToDataStream(dataStream);
    return dataStream;
}


QDataStream &operator>>(QDataStream &dataStream, SerialPortSettingsConfigset &object)
{
    object.extractFromDataStream(dataStream);
    return dataStream;
}
