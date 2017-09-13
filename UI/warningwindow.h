#ifndef WARNINGWINDOW_H
#define WARNINGWINDOW_H

#include <QDialog>
#include <qdebug.h>

namespace Ui {
class WarningWindow;
}

class WarningWindow : public QDialog
{
    Q_OBJECT

public:
    static WarningWindow* getInstance()
    {
        static WarningWindow instance;
        return &instance;
    }

    void setWarningText(QString text);
    void showCancelButton(bool status);
    void enableOKButton(bool status);
    bool yesOrNo;
    QString systemLanguage;

private:
    Ui::WarningWindow *ui;

    explicit WarningWindow(QWidget *parent = 0);
    ~WarningWindow();

private slots:
    void on_cancel_pressed();

    void on_cancel_released();

    void on_ok_pressed();

    void on_ok_released();
};

#endif // WARNINGWINDOW_H
