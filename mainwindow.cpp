#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "serialportsettingsdialog.h"

#include <math.h>
#include <QMatrix>

#include <QDebug>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->resize(1200, 600);

    thread = new MyThread(this);


    ui->connectPushButton->setEnabled(true);
    ui->disconnectPushButton->setEnabled(false);


    ui->integraMethodRadioButton->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete ui;

}

QSize MainWindow::sizeHint() const
{
    return QSize(100,800);
}





void MainWindow::on_wheelDiameterLineEdit_editingFinished()
{

    float wheelDiameter = ui->wheelDiameterLineEdit->text().toDouble();
    ui->glWidget->setWheelDiameter(wheelDiameter/1000.0f);
}

void MainWindow::on_pulsesPerRevLineEdit_editingFinished()
{
    float pulsesPerRevolution = ui->pulsesPerRevLineEdit->text().toDouble();
    ui->glWidget->setPulsesPerRevolution(pulsesPerRevolution);
}

void MainWindow::on_axleLengthLineEdit_editingFinished()
{
    float axleLength = ui->axleLengthLabel->text().toDouble();
    ui->glWidget->setAxleLength(axleLength);
}

void MainWindow::on_actionSerialPort_triggered()
{
    SerialPortSettingsDialog* dialog = new SerialPortSettingsDialog(this);

    dialog->setModal(true);

    dialog->exec();
}

void MainWindow::on_connectPushButton_clicked()
{
    thread->serialOpen();

    connect(thread, SIGNAL(dataDecoded(int*)), ui->glWidget, SLOT(addEncoderPulesVrVl(int*)));

    thread->setFlatStop(false);
    thread->start();

    ui->connectPushButton->setEnabled(false);
    ui->disconnectPushButton->setEnabled(true);
}

void MainWindow::on_disconnectPushButton_clicked()
{
    thread->setFlatStop(true);

    thread->serialClose();

    ui->connectPushButton->setEnabled(true);
    ui->disconnectPushButton->setEnabled(false);
}

void MainWindow::on_integraMethodRadioButton_toggled(bool checked)
{
    if(checked == true)
    {
        ui->glWidget->setDeadReckoningMethod(GLWidget::INTEGRAL);
    }
}

void MainWindow::on_simplerMethodRadioButton_clicked(bool checked)
{
    if(checked == true)
    {
        ui->glWidget->setDeadReckoningMethod(GLWidget::SIMPLE);
    }
}

void MainWindow::on_actionNew_triggered()
{
    ui->glWidget->resetDeadReckoning();

    ui->wheelDiameterLineEdit->setText(QString::number(ui->glWidget->getWheelDiameter()));
    ui->pulsesPerRevLineEdit->setText(QString::number(ui->glWidget->getPulsesPerRevolution()));
    ui->axleLengthLabel->setText(QString::number(ui->glWidget->getAxleLength()));
}
