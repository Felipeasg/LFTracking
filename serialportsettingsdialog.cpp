#include "serialportsettingsdialog.h"
#include "ui_serialportsettingsdialog.h"

#include "serialportsettingmodel.h"

#include <QSerialPortInfo>

SerialPortSettingsDialog::SerialPortSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialPortSettingsDialog)
{
    ui->setupUi(this);

    foreach (QSerialPortInfo qSerialInfo, QSerialPortInfo::availablePorts()) {

        qDebug() << qSerialInfo.portName();
        ui->portNamesComboBox->addItem(qSerialInfo.portName());
    }

    int idx = 0;
    for(int i = 0; i < ui->portNamesComboBox->count(); i++)
    {
        if(QString::compare(SerialPortSettingModel::getInstance()->getPortName(),
                            ui->portNamesComboBox->itemText(i)) == 0)
        {
            idx = i;
        }

    }

    ui->portNamesComboBox->setCurrentIndex(idx);

    for(int i = 0; i < ui->baudRateComboBox->count(); i++)
    {
        if(SerialPortSettingModel::getInstance()->getBaudRate() == ui->baudRateComboBox->itemText(i).toInt())
        {
            idx = i;
        }
    }

    ui->baudRateComboBox->setCurrentIndex(idx);

    ui->dataBitsComboBox->setCurrentIndex(3);
}

SerialPortSettingsDialog::~SerialPortSettingsDialog()
{
    delete ui;
}

void SerialPortSettingsDialog::on_okPushButton_clicked()
{
    int idx = ui->portNamesComboBox->currentIndex();
    SerialPortSettingModel::getInstance()->setPortName(ui->portNamesComboBox->itemText(idx));

    idx = ui->baudRateComboBox->currentIndex();
    SerialPortSettingModel::getInstance()->setBaudRate(ui->baudRateComboBox->itemText(idx).toInt());

    this->close();
}

void SerialPortSettingsDialog::on_cancelPushButton_clicked()
{
    this->close();
}
