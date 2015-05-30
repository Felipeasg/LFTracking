#ifndef SERIALPORTSETTINGMODEL_H
#define SERIALPORTSETTINGMODEL_H

#include <QObject>

#include "serialportsettingsconfigset.h"

class SerialPortSettingModel : public QObject
{
    Q_OBJECT
public:
    explicit SerialPortSettingModel(QObject *parent = 0);
    ~SerialPortSettingModel();

    static SerialPortSettingModel* getInstance();

    int getBaudRate() const;
    void setBaudRate(int value);

    int getDataBits() const;
    void setDataBits(int value);

    int getParity() const;
    void setParity(int value);

    int getStopBits() const;
    void setStopBits(int value);

    int getFlowControl() const;
    void setFlowControl(int value);

    SerialPortSettingsConfigset getSerialPortConfigSet() const;
    void setSerialPortConfigSet(const SerialPortSettingsConfigset &value);

    QString getPortName() const;
    void setPortName(const QString &value);

signals:

public slots:

private:

    static SerialPortSettingModel* uniqueInstance;
    SerialPortSettingsConfigset serialPortConfigSet;
};

#endif // SERIALPORTSETTINGMODEL_H
