#include "receivedata.h"

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

ReceiveData::ReceiveData(QObject *parent) : QObject(parent)
{
    serialPort = new QSerialPort(this);
    inBuffer = new Buffer(inBufferArray, 65536);
    this->decoderState = CODEC_SYNC;
}

ReceiveData::~ReceiveData()
{

}

bool ReceiveData::serialPortConnect()
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

        return true;
    }
    else
    {
        return false;

        qDebug() << "Could not open";
    }

}

void ReceiveData::serialPortDisconnect()
{
    serialPort->close();

    disconnect(serialPort, SIGNAL(readyRead()), this, SLOT(serialReceived()));
}

void ReceiveData::serialReceived()
{
    int result;
    QByteArray serialData = serialPort->readAll();
    inBuffer->putN(serialData.data(), 0, serialData.length());

    do
    {
        result = this->decode();
    }while(result != CODEC_NEED_DATA);
}

int ReceiveData::decode()
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

bool ReceiveData::processPreamble()
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

bool ReceiveData::processData()
{
        //static int i = 0;
        inBuffer->getN((char *)payloadDecoded, 0, PAYLOAD_SIZE);

        //qDebug() << "received - " << i;
        //i++;
        emit dataDecoded(payloadDecoded);
        return true;
}

