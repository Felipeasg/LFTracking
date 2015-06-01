#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "serialportsettingsdialog.h"

#include <math.h>
#include <QMatrix>

#include <QDebug>

#include <QMessageBox>

#include <serialportsettingmodel.h>

#define DEAD_RECKONING 0
#define PLOT           1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->resize(1200, 600);

    receiveData =  new ReceiveData(this);

    ui->connectPushButton->setEnabled(true);
    ui->disconnectPushButton->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

    timeElapsed = 0;
    ui->integraMethodRadioButton->setChecked(true);
    ui->deadReckoningRadioButton->setChecked(true);

    connect(this, SIGNAL(dataDecoded(int*)), ui->glWidget, SLOT(addEncoderPulesVrVl(int*)));

    ui->plotWidget->hide();
    ui->plotWidget->resize(500,300);


    this->showPointsOnGraph = false;
    ui->plotWidget->addGraph(); // blue line
    ui->plotWidget->graph(0)->setPen(QPen(Qt::blue));
    ui->plotWidget->graph(0)->setAntialiasedFill(false);
    ui->plotWidget->addGraph(); // red line
    ui->plotWidget->graph(1)->setPen(QPen(Qt::red));
    ui->plotWidget->graph(0)->setChannelFillGraph(ui->plotWidget->graph(1));

    ui->plotWidget->addGraph(); // blue dot
    ui->plotWidget->graph(2)->setPen(QPen(Qt::blue));
    ui->plotWidget->graph(2)->setLineStyle(QCPGraph::lsNone);
    ui->plotWidget->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->plotWidget->addGraph(); // red dot
    ui->plotWidget->graph(3)->setPen(QPen(Qt::red));
    ui->plotWidget->graph(3)->setLineStyle(QCPGraph::lsNone);
    ui->plotWidget->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

    ui->plotWidget->xAxis->setRange(0, 30, Qt::AlignLeft);

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    maxYAxis = FLT_MIN;
    minYAxis = FLT_MAX;

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

void MainWindow::on_connectPushButton_clicked()
{

    if(receiveData->serialPortConnect())
    {        
        qDebug() << "Opened";

        ui->connectPushButton->setEnabled(false);
        ui->actionConnect->setEnabled(false);
        ui->disconnectPushButton->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);


        ui->consoleTextEdit->append("Serial port " + SerialPortSettingModel::getInstance()->getPortName() + " is open!");
        ui->consoleTextEdit->append("Baudrate: " + QString::number(SerialPortSettingModel::getInstance()->getBaudRate()));

    }
    else
    {


        ui->consoleTextEdit->append("Serial port " + SerialPortSettingModel::getInstance()->getPortName() + " could not be opened");
    }
}

void MainWindow::on_disconnectPushButton_clicked()
{
    receiveData->serialPortDisconnect();
    ui->connectPushButton->setEnabled(true);
    ui->actionConnect->setEnabled(true);
    ui->disconnectPushButton->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);

    ui->consoleTextEdit->append("Serial port " + SerialPortSettingModel::getInstance()->getPortName() + " is close!");
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


    ui->wheelDiameterLineEdit->setText(QString::number(ui->glWidget->getWheelDiameter()));
    ui->pulsesPerRevLineEdit->setText(QString::number(ui->glWidget->getPulsesPerRevolution()));
    ui->axleLengthLabel->setText(QString::number(ui->glWidget->getAxleLength()));
}

void MainWindow::on_deadReckoningRadioButton_toggled(bool checked)
{
    if(checked == true)
    {
        this->viewSetting = DEAD_RECKONING;

        ui->plotWidget->hide();
        ui->glWidget->show();

        if(encoderLList.count() > 0 && encoderRList.count() > 0)
        {
            ui->glWidget->resetDeadReckoning();

            int count = encoderLList.count();
            for(int i = 0; i < count; i++)
            {
                ui->glWidget->addPulsesVrVl(encoderRList[i], encoderLList[i]);
            }

            ui->glWidget->update();
        }
        connect(receiveData, SIGNAL(dataDecoded(int*)), ui->glWidget, SLOT(addEncoderPulesVrVl(int*)));
        disconnect(receiveData, SIGNAL(dataDecoded(int*)), this, SLOT(plotVariables(int*)));
    }
}

void MainWindow::on_plotRadioButton_toggled(bool checked)
{
    if(checked == true)
    {
        this->viewSetting = PLOT;

        ui->plotWidget->show();
        ui->glWidget->hide();

        if(ui->glWidget->encoderListCount() > 0)
        {
            int count = ui->glWidget->encoderListCount();
            int key = 0;
            encoderLList.clear();
            encoderRList.clear();
            for(int i = 0; i < count; i++)
            {
                double value0 = (double)ui->glWidget->getEncoderRList()[i];
                double value1 = (double)ui->glWidget->getEncoderLList()[i];

                // add data to lines:
                  ui->plotWidget->graph(0)->addData(key, value0);
                  ui->plotWidget->graph(1)->addData(key, value1);

                  if(this->showPointsOnGraph)
                  {
                      ui->plotWidget->graph(2)->addData(key, value0);
                      //                 //encoderLList.append(ui->glWidget->getEncoderLList()[i]);
                      //encoderRList.append(ui->glWidget->getEncoderRList()[i]);         ui->plotWidget->graph(3)->clearData();
                      ui->plotWidget->graph(3)->addData(key, value1);
                  }


                  key++;
            }

            ui->plotWidget->xAxis->setRange(0, key+10);

            ui->plotWidget->replot();
        }

        connect(receiveData, SIGNAL(dataDecoded(int*)), this, SLOT(plotVariables(int*)));
        disconnect(receiveData, SIGNAL(dataDecoded(int*)), ui->glWidget, SLOT(addEncoderPulesVrVl(int*)));
    }
}

void MainWindow::plotVariables(int *variables)
{
    for(int i = 0; i < 200; i = i + 2)
    {
        encoderLList.append(variables[i+1]);
        encoderRList.append(variables[i]);
        double value0 = (double)variables[i+1];
        double value1 = (double)variables[i];

        maxYAxis = qMax(qMax(value0,value1),maxYAxis);
        minYAxis = qMin(qMin(value0,value1),minYAxis);
        // add data to lines:
          ui->plotWidget->graph(0)->addData(timeElapsed, value0);
          ui->plotWidget->graph(1)->addData(timeElapsed, value1);

          if(this->showPointsOnGraph)
          {
              ui->plotWidget->graph(2)->addData(timeElapsed, value0);
              ui->plotWidget->graph(3)->addData(timeElapsed, value1);
          }

          timeElapsed++;
    }

    ui->plotWidget->xAxis->setRange(0, timeElapsed+10);
    ui->plotWidget->yAxis->setRange(minYAxis*1.05, maxYAxis*1.05);


    ui->plotWidget->replot();

}

void MainWindow::on_actionClear_triggered()
{
    ui->plotWidget->graph(0)->clearData();
    ui->plotWidget->graph(1)->clearData();
    ui->plotWidget->graph(2)->clearData();
    ui->plotWidget->graph(3)->clearData();

    this->encoderLList.clear();
    this->encoderRList.clear();

    maxYAxis = FLT_MIN;
    minYAxis = FLT_MAX;

    timeElapsed = 0;

    ui->plotWidget->xAxis->setRange(0, 30, Qt::AlignLeft);

    ui->plotWidget->replot();
}



void MainWindow::on_actionShow_points_triggered(bool checked)
{
    this->showPointsOnGraph = checked;
}

void MainWindow::on_actionNew_tracking_triggered()
{
    ui->glWidget->resetDeadReckoning();
}

void MainWindow::on_actionConnect_triggered()
{
    if(receiveData->serialPortConnect())
    {
        qDebug() << "Opened";

        ui->connectPushButton->setEnabled(false);
        ui->actionConnect->setEnabled(false);
        ui->disconnectPushButton->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);

        ui->consoleTextEdit->append("Serial port " + SerialPortSettingModel::getInstance()->getPortName() + " is open!");
        ui->consoleTextEdit->append("Baudrate: " + QString::number(SerialPortSettingModel::getInstance()->getBaudRate()));

    }
    else
    {
        ui->consoleTextEdit->append("Serial port " + SerialPortSettingModel::getInstance()->getPortName() + " could not be opened");
    }
}

void MainWindow::on_actionDisconnect_triggered()
{
    receiveData->serialPortDisconnect();
    ui->connectPushButton->setEnabled(true);
    ui->actionConnect->setEnabled(true);
    ui->disconnectPushButton->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);

    ui->consoleTextEdit->append("Serial port " + SerialPortSettingModel::getInstance()->getPortName() + " is close!");
}

void MainWindow::on_actionSettings_triggered()
{
    SerialPortSettingsDialog* dialog = new SerialPortSettingsDialog(this);

    dialog->setModal(true);

    dialog->exec();
}
