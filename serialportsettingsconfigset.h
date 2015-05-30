#ifndef SERIALPORTSETTINGSCONFIGSET_H
#define SERIALPORTSETTINGSCONFIGSET_H

#include <QSerialPort>


#include <QtCore>
#include <QtCore/QByteArray>
#include <QtCore/qvariant.h>

class SerialPortSettingsConfigset
{
public:
    SerialPortSettingsConfigset();
    ~SerialPortSettingsConfigset();

    QString portName;
    int baudRate;
    int dataBits;
    int parity;
    int stopBits;
    int flowControl;

    void insertToDataStream(QDataStream& dataStream) const;
    void extractFromDataStream(QDataStream& dataStream);
};

QDataStream& operator<<(QDataStream& dataStream, const SerialPortSettingsConfigset& object);
QDataStream& operator>>(QDataStream& dataStream, SerialPortSettingsConfigset& object);

#endif // SERIALPORTSETTINGSCONFIGSET_H
