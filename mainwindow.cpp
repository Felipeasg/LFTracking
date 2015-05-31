#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "serialportsettingsdialog.h"

#include <math.h>
#include <QMatrix>

#include <QDebug>

#include <QMessageBox>

#include <serialportsettingmodel.h>

#define CODEC_NEED_DATA 1
#define CODEC_OK        4
#define CODEC_SYNC      2
#define CODEC_PAYLOAD   3

#define PREAMBLE_SIZE   4 //in bytes
#define PAYLOAD_SIZE    800 //in bytes

#define PREAMBLE_1  0xAA
#define PREAMBLE_2  0xAA
#define PREAMBLE_3  0xAA
#define PREAMBLE_4  0xAA

#define DEAD_RECKONING 0
#define PLOT           1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->resize(1200, 600);

    thread = new MyThread(this);

    serialPort = new QSerialPort(this);

    inBuffer = new Buffer(inBufferArray, 65536);


    this->decoderState = CODEC_SYNC;

    ui->connectPushButton->setEnabled(true);
    ui->disconnectPushButton->setEnabled(false);


    ui->integraMethodRadioButton->setChecked(true);
    ui->deadReckoningRadioButton->setChecked(true);

    connect(this, SIGNAL(dataDecoded(int*)), ui->glWidget, SLOT(addEncoderPulesVrVl(int*)));

    ui->plotWidget->hide();
    ui->plotWidget->resize(500,300);


    ui->plotWidget->addGraph(); // blue line
    ui->plotWidget->graph(0)->setPen(QPen(Qt::blue));
    ui->plotWidget->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
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

//    ui->plotWidget->xAxis->setTickLabelType(QCPAxis::ltDateTime);
//    ui->plotWidget->xAxis->setDateTimeFormat("hh:mm:ss");
//    ui->plotWidget->xAxis->setAutoTickStep(false);
//    ui->plotWidget->xAxis->setTickStep(2);
//    ui->plotWidget->axisRect()->setupFullAxesBox();

    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->plotWidget->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plotWidget->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->plotWidget->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plotWidget->yAxis2, SLOT(setRange(QCPRange)));

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
    serialPort->setPortName(SerialPortSettingModel::getInstance()->getPortName());
    qDebug() << serialPort->portName();
    serialPort->setBaudRate(SerialPortSettingModel::getInstance()->getBaudRate());
    qDebug() << serialPort->baudRate();
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);



    if(serialPort->open(QIODevice::ReadWrite))
    {
        connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialReceived()));

        qDebug() << "Opened";

        ui->connectPushButton->setEnabled(false);
        ui->disconnectPushButton->setEnabled(true);

        ui->consoleTextEdit->append("Serial port " + serialPort->portName() + " is open!");
        ui->consoleTextEdit->append("Baudrate: " + QString::number(serialPort->baudRate()));

    }
    else
    {
        qDebug() << "Could not open";

        ui->consoleTextEdit->append("Serial port " + serialPort->portName() + " could not be opened");
    }


#if 0
    thread->serialOpen();

    connect(thread, SIGNAL(dataDecoded(int*)), ui->glWidget, SLOT(addEncoderPulesVrVl(int*)));

    thread->setFlatStop(false);
    thread->start();

    ui->connectPushButton->setEnabled(false);
    ui->disconnectPushButton->setEnabled(true);
#endif
}

void MainWindow::on_disconnectPushButton_clicked()
{
    serialPort->close();

    disconnect(serialPort, SIGNAL(readyRead()), this, SLOT(serialReceived()));

    ui->connectPushButton->setEnabled(true);
    ui->disconnectPushButton->setEnabled(false);

    ui->consoleTextEdit->append("Serial port " + serialPort->portName() + " is close!");
#if 0
    thread->setFlatStop(true);

    thread->serialClose();

    ui->connectPushButton->setEnabled(true);
    ui->disconnectPushButton->setEnabled(false);
#endif
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

void MainWindow::serialReceived()
{
    int result;
    QByteArray serialData = serialPort->readAll();
    inBuffer->putN(serialData.data(), 0, serialData.length());

    do
    {
        result = this->decode();
    }while(result != CODEC_NEED_DATA);
}

int MainWindow::decode()
{
    switch (this->decoderState) {
    case CODEC_SYNC:

        if(inBuffer->getLength() < PREAMBLE_SIZE)
        {
            return CODEC_NEED_DATA;
        }

        if(processPreamble())
        {
            this->decoderState = CODEC_PAYLOAD;

            if(inBuffer->getLength() < PAYLOAD_SIZE)
            {
                return CODEC_NEED_DATA;
            }
            else
            {
                this->decoderState = CODEC_SYNC;
                processData();
            }
        }

        break;

    case CODEC_PAYLOAD:

        if(inBuffer->getLength() < PAYLOAD_SIZE)
        {
            return CODEC_NEED_DATA;
        }
        else
        {
            this->decoderState = CODEC_SYNC;
            processData();
        }
        break;

    default:
        break;
    }

    return CODEC_OK;
}

bool MainWindow::processPreamble()
{
    char temp_data = 0;
    bool bresult = false;
    inBuffer->getByte(&temp_data);

    if((unsigned char)temp_data == PREAMBLE_1)
    {
        inBuffer->getByte(&temp_data);

        if((unsigned char)temp_data == PREAMBLE_2)
        {
            inBuffer->getByte(&temp_data);

            if((unsigned char)temp_data == PREAMBLE_3)
            {
                inBuffer->getByte(&temp_data);

                if((unsigned char)temp_data == PREAMBLE_3)
                {
                    bresult = true;
                }
            }
        }
    }

    return bresult;
}

bool MainWindow::processData()
{
//    static int i = 0;
    inBuffer->getN((char *)payloadDecoded, 0, PAYLOAD_SIZE);

//    qDebug() << "received - " << i;
//    i++;
    emit dataDecoded(payloadDecoded);
    return true;
}

void MainWindow::on_deadReckoningRadioButton_toggled(bool checked)
{
    if(checked == true)
    {
        this->viewSetting = DEAD_RECKONING;

        ui->plotWidget->hide();
        ui->glWidget->show();

        connect(this, SIGNAL(dataDecoded(int*)), ui->glWidget, SLOT(addEncoderPulesVrVl(int*)));
        disconnect(this, SIGNAL(dataDecoded(int*)), this, SLOT(plotVariables(int*)));
    }
}

void MainWindow::on_plotRadioButton_toggled(bool checked)
{
    if(checked == true)
    {
        this->viewSetting = PLOT;

        ui->plotWidget->show();
        ui->glWidget->hide();

        connect(this, SIGNAL(dataDecoded(int*)), this, SLOT(plotVariables(int*)));
        disconnect(this, SIGNAL(dataDecoded(int*)), ui->glWidget, SLOT(addEncoderPulesVrVl(int*)));
    }
}

void MainWindow::plotVariables(int *variables)
{
//    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    static int key = 0;

//    static double lastPointKey = 0;

    for(int i = 0; i < 100; i = i + 2)
    {
        double value0 = (double)variables[i+1]; //qSin(key*1.6+qCos(key*1.7)*2)*10 + qSin(key*1.2+0.56)*20 + 26;
        double value1 = (double)variables[i]; //qSin(key*1.3+qCos(key*1.2)*1.2)*7 + qSin(key*0.9+0.26)*24 + 26;

        // add data to lines:
          ui->plotWidget->graph(0)->addData(key, value0);
          ui->plotWidget->graph(1)->addData(key, value1);

          ui->plotWidget->graph(0)->rescaleValueAxis();
          ui->plotWidget->graph(1)->rescaleValueAxis(true);
//          // set data of dots:
//          ui->plotWidget->graph(2)->clearData();
//          ui->plotWidget->graph(2)->addData(key, value0);
//          ui->plotWidget->graph(3)->clearData();
//          ui->plotWidget->graph(3)->addData(key, value1);
//          // remove data of lines that's outside visible range:
//          ui->plotWidget->graph(0)->removeDataBefore(key-8);
//          ui->plotWidget->graph(1)->removeDataBefore(key-8);
//          // rescale value (vertical) axis to fit the current data:
//          ui->plotWidget->graph(0)->rescaleValueAxis();
//          ui->plotWidget->graph(1)->rescaleValueAxis(true);
//          lastPointKey = key;

          key++;
    }

    ui->plotWidget->xAxis->setRange(0, key+10);
    // make key axis range scroll with the data (at a constant range size of 8):
//    ui->plotWidget->xAxis->setRange(0, 30, Qt::AlignRight);
//    ui->plotWidget->xAxis->setRange(0, 8 + key, Qt::AlignLeft);
    ui->plotWidget->replot();

}
