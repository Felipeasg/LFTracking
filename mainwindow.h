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

private:
    Ui::MainWindow *ui;

    MyThread* thread;

};

#endif // MAINWINDOW_H
