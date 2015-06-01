#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

#include "buffer.h"
#include "receivedata.h"

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

    void on_connectPushButton_clicked();

    void on_disconnectPushButton_clicked();

    void on_integraMethodRadioButton_toggled(bool checked);

    void on_simplerMethodRadioButton_clicked(bool checked);

    void on_actionNew_triggered();

    void on_deadReckoningRadioButton_toggled(bool checked);

    void on_plotRadioButton_toggled(bool checked);

    void plotVariables(int* variables);

    void on_actionClear_triggered();



    void on_actionShow_points_triggered(bool checked);

    void on_actionNew_tracking_triggered();

    void on_actionConnect_triggered();

    void on_actionDisconnect_triggered();

    void on_actionSettings_triggered();

private:



    Ui::MainWindow *ui;

    int viewSetting;

    ReceiveData* receiveData;

    bool showPointsOnGraph;

    QList<int> encoderRList;
    QList<int> encoderLList;

    int timeElapsed;

};

#endif // MAINWINDOW_H
