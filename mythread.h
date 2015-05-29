#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QtCore>
#include <QSerialPort>

#include "buffer.h"

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);
    ~MyThread();

    void run();
    QSerialPort* serialPort;

signals:
    void dataDecoded(int32_t* encoders);

public slots:
    void serialReceived();

private:

    enum DecodeState
    {
        PREAMBLE,
        PAYLOAD,
        DECODED
    };

    Buffer* inBuffer;
    char inBufferArray[65536];
    int32_t encoders[200];
    DecodeState state;


};

#endif // MYTHREAD_H
