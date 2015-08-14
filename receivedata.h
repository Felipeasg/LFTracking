#ifndef RECEIVEDATA_H
#define RECEIVEDATA_H

#include <QObject>
#include <QSerialPort>

#include "buffer.h"
#include "serialportsettingmodel.h"

class ReceiveData : public QObject
{
    Q_OBJECT
public:
    explicit ReceiveData(QObject *parent = 0);
    ~ReceiveData();

    bool serialPortConnect();
    void serialPortDisconnect();
    void reset();

signals:
    void dataDecoded(int* decodedData);

public slots:
    void serialReceived();
    void timeout();

private:

    int decode();
    bool processPreamble();
    bool processData();


    QSerialPort* serialPort;

    Buffer* inBuffer;
    char inBufferArray[65536];

    int decoderState;

    int payloadDecoded[200];

    int count;

    QTimer* timeoutTimer;
};

#endif // RECEIVEDATA_H
