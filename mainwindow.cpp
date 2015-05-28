#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <math.h>
#include <QMatrix>

#include <QDebug>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);




//    timer = new QTimer(this);




    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(on_timerElapsed()));
//    timer->start(100);

    serial = new QSerialPort(this);

    //https://www.youtube.com/watch?v=UD78xyKbrfk
    serial->setPortName("ttyUSB0");
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if(serial->open(QIODevice::ReadWrite))
    {
        qDebug() << "Opened";
    }
    else
    {
        qDebug() << "Could not open";
    }


    connect(serial, SIGNAL(readyRead()), this, SLOT(serialReceived()));
    connect(this, SIGNAL(pointsadded()), this, SLOT(on_drawPoints()));
    connect(this, SIGNAL(drawnow()), ui->glWidget, SLOT(update()));

}

MainWindow::~MainWindow()
{
    delete ui;

}


char dataPkg[804];
int32_t encoders[201];
//QList<int> listpointVR;
//QList<int> listpointVL;

int state = 0;
int sizeOfPack = 0;
int32_t count = 0;
QByteArray byteArray;

void MainWindow::serialReceived()
{
    switch (state) {

    case 0:

        byteArray = serial->readAll();


        if((unsigned char)byteArray[0] == 0xaa && (unsigned char)byteArray[1] == 0xaa)
        {
            if((unsigned char)byteArray[2] == 0xaa && (unsigned char)byteArray[3] == 0xaa)
            {
                state = 1;
                sizeOfPack = 0;

                byteArray.remove(0, 4);
            }
        }


        break;

    case 1:

        byteArray += serial->readAll();

        if(byteArray.count() > 800)
        {
            memcpy(encoders, byteArray, 800);
            byteArray.remove(0, 800);
            state = 2;            
        }

        break;

    case 2:

        emit pointsadded();

        sizeOfPack = 0;
        state = 0;
        break;

    default:
        break;
    }


#if 0
    static qint64 count = 0;
    char data[804];


    int size = serial->read(&data[count], 100);


//    qDebug() << "reading " << size << "bytes";
    if(count >= 804)
    {
        qDebug() << "reading data";

        memcpy(encoders, data, 804);

        for(int i = 0; i < 201; i++)
        {
            qDebug() << encoders[i];
        }



        if(encoders[200] == 5555)
        {
            qDebug() << "Read finished!";
//            emit pointsadded();
        }

        count -= 804;
    }

    count += size;
#endif

}

void MainWindow::on_MainWindow_destroyed()
{

}

void MainWindow::on_drawPoints()
{

    for(int i = 0; i < 102; i = i + 2)
    {
//        qDebug() << i << encoders[i] << " " << encoders[i+1];
//        ui->glWidget->vrList.append(encoders[i]);
//        ui->glWidget->vlList.append(encoders[i+1]);
//        ui->glWidget->vrList.append(listpointVR[i]);
//        ui->glWidget->vrList.append(listpointVL[i]);
        ui->glWidget->addPulsesVrVl(encoders[i+1], encoders[i]);
    }
//    listpointVR.clear();
//    listpointVL.clear();

    emit drawnow();
}

void MainWindow::on_timerElapsed()
{
    for(int i = 0; i < 100; i++)
    {
        ui->glWidget->addPulsesVrVl(100, 101);
    }

    emit drawnow();
}
