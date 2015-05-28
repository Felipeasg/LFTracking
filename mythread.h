#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QtCore>
#include <QSerialPort>

class MyThread : public QThread
{
public:
    MyThread();
    ~MyThread();

    void run();
    QSerialPort* serialPort;

private:

};

#endif // MYTHREAD_H
