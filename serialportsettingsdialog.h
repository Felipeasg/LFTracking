#ifndef SERIALPORTSETTINGSDIALOG_H
#define SERIALPORTSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SerialPortSettingsDialog;
}

class SerialPortSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialPortSettingsDialog(QWidget *parent = 0);
    ~SerialPortSettingsDialog();

private slots:
    void on_okPushButton_clicked();

    void on_cancelPushButton_clicked();

private:
    Ui::SerialPortSettingsDialog *ui;
};

#endif // SERIALPORTSETTINGSDIALOG_H
