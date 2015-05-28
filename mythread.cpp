#include "mythread.h"

MyThread::MyThread()
{

}

MyThread::~MyThread()
{

}

void MyThread::run()
{
    char data[804];
    int count = 0;

    while(1)
    {


        while(count < 804)
        {
            int size = serialPort->read(&data[count],804);
            qDebug() << "Read = " << size;
            count += size;
        }
        count -= 804;

        for(int i = 0; i < 804; i++)
        {
            qDebug() << data[i];
        }


    }

}

