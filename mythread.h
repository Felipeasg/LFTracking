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

    void serialOpen();
    void serialClose();

    QSerialPort *getSerialPort() const;
    void setSerialPort(QSerialPort *value);

    bool getFlatStop() const;
    void setFlatStop(bool value);

signals:
    void dataDecoded(int* encoders);

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
    int encoders[200];
    DecodeState state;

    QSerialPort* serialPort;

    bool flatStop;

};

#endif // MYTHREAD_H
