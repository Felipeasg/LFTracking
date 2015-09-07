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

    timeoutTimer = new QTimer(this);
    timeoutTimer->setInterval(380);
    connect(timeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));
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

void ReceiveData::reset()
{
    serialPort->readAll();

    char b;
    while(inBuffer->getLength())
    {
        inBuffer->getByte(&b);
    }
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
//            qDebug() << "N";
            return CODEC_NEED_DATA;
        }

        if(processPreamble())
        {
            timeoutTimer->start();
//            qDebug() << count << "P";
            this->decoderState = CODEC_PAYLOAD;

            if(inBuffer->getLength() < PAYLOAD_SIZE)
            {
//                qDebug() << "N";
                return CODEC_NEED_DATA;
            }
            else
            {
                this->decoderState = CODEC_SYNC;

                timeoutTimer->stop();
                processData();
            }
        }

        break;

    case CODEC_PAYLOAD:

        if(inBuffer->getLength() < PAYLOAD_SIZE)
        {
//            qDebug() << "N";
            return CODEC_NEED_DATA;

        }
        else
        {
            //FIXME: tem um bug na recepção dos dados, quando o carrinho é resetado e a interface é mantida sem clicar em
            //nada, de alguma forma os dados são plotados, eu verifiquei e os dados chegam corretamente na porta serial
            //de alguma forma estes dados não são passados para o mainwidget a classe que se registra no evento de dados
            //recebidos. O valor plotado fica zero e da um pico em 1.25e-9

            //Bug descoberto
            //Ele se deve ao fato de qdo é pressionado reset no cara que envia o pacote, ele vai reenviar o cabeçalho
            //como o cabeçalho é 0xAAAAAAAA isso dá um valor muito grande, quando o gráfico ajusta a escala não
            //da então para ver os dados reais. A solução é implementar um timeout, se os dados não chegarem reseta
            //o codec para CODEC SYNC e descarta os dados
//            qDebug() << count << "D";
            this->decoderState = CODEC_SYNC;            
            processData();

            count++;
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

        memcpy(dataconverted, payloadDecoded, PAYLOAD_SIZE);
        //qDebug() << "received - " << i;
        //i++;
//        for(int i = 0; i < 200; i++)
//        {
//            qDebug() << payloadDecoded[i];
//        }
        emit dataDecoded(dataconverted);
        return true;
}

void ReceiveData::timeout()
{
    this->decoderState = CODEC_SYNC;
    serialPort->readAll();
    char n;
    while(inBuffer->getLength() > 0)
    {
        inBuffer->getByte(&n);
    }
    //qDebug() << "Timeout";

    timeoutTimer->stop();
}

