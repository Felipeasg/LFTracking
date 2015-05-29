#include "mythread.h"

MyThread::MyThread(QObject *parent):
    QThread(parent)
{
    inBuffer = new Buffer(inBufferArray, 65536);

    serialPort = new QSerialPort(this);

    //https://www.youtube.com/watch?v=UD78xyKbrfk
    serialPort->setPortName("ttyUSB0");
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if(serialPort->open(QIODevice::ReadWrite))
    {
        qDebug() << "Opened";
    }
    else
    {
        qDebug() << "Could not open";
    }


    connect(serialPort, SIGNAL(readyRead()), this, SLOT(serialReceived()));
}

MyThread::~MyThread()
{

}

void MyThread::run()
{
    char preamble[4];
    char payload[800];
#ifdef COUNT_PACKAGES
    static int count = 0;
#endif

    while(1)
    {
        switch (state) {

        case PREAMBLE:

            if(inBuffer->getLength() > 4)
            {
                inBuffer->getN(preamble,0, 4);
                if((unsigned char)preamble[0] == 0xaa &&
                        (unsigned char)preamble[1] == 0xaa &&
                        (unsigned char)preamble[2] == 0xaa &&
                        (unsigned char)preamble[3] == 0xaa)
                {
                    state = PAYLOAD;
                }

//                if((unsigned char)preamble[0] == 0xaa)
//                {
//                    inBuffer->getByte(preamble);
//                    if((unsigned char)preamble[0] == 0xaa)
//                    {
//                        inBuffer->getByte(preamble);
//                        if((unsigned char)preamble[0] == 0xaa)
//                        {
//                            inBuffer->getByte(preamble);
//                            if((unsigned char)preamble[0] == 0xaa)
//                            {
//                                state = PAYLOAD;
//                            }
//                        }
//                    }
//                }

            }

            break;

        case PAYLOAD:

            if(inBuffer->getLength() >= 800)
            {
                inBuffer->getN(payload, 0, 800);
                memcpy(encoders, payload, 800);

#ifdef COUNT_PACKAGES
                qDebug() << count;

                count++;
#endif
                emit dataDecoded(encoders);

                state = PREAMBLE;
            }
            break;

        case DECODED:


            break;

        default:
            break;
        }
    }

}

void MyThread::serialReceived()
{
    QByteArray serialData = serialPort->readAll();

    inBuffer->putN(serialData.data(), 0, serialData.length());


//    qDebug() << "Data";
//    foreach (char data, serialData) {
//        qDebug() << (int)data;
//    }
}

