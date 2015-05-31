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

    QSize sizeHint() const;

public slots:

signals:

    void dataDecoded(int* decodedData);

private slots:
    void on_wheelDiameterLineEdit_editingFinished();

    void on_pulsesPerRevLineEdit_editingFinished();



    void on_axleLengthLineEdit_editingFinished();

    void on_actionSerialPort_triggered();

    void on_connectPushButton_clicked();

    void on_disconnectPushButton_clicked();

    void on_integraMethodRadioButton_toggled(bool checked);

    void on_simplerMethodRadioButton_clicked(bool checked);

    void on_actionNew_triggered();

    void serialReceived();



    void on_deadReckoningRadioButton_toggled(bool checked);

    void on_plotRadioButton_toggled(bool checked);

    void plotVariables(int* variables);

private:

    int decode();
    bool processPreamble();
    bool processData();

    Ui::MainWindow *ui;

    MyThread* thread;

    QSerialPort* serialPort;

    Buffer* inBuffer;
    char inBufferArray[65536];

    int decoderState;

    int payloadDecoded[200];

    int viewSetting;

};

#endif // MAINWINDOW_H
