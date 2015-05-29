#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "mythread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void serialReceived();
    void on_MainWindow_destroyed();
    void on_drawPoints();
    void on_timerElapsed();

    void readData();

signals:
    void drawnow();
    void pointsadded();
private:
    Ui::MainWindow *ui;

    QTimer* timer;

    float b;
    float vr, vl;
    float t;
    float initialAngle;
    float x0, y0;

    MyThread* thread;

    QSerialPort* serial;
};

#endif // MAINWINDOW_H
