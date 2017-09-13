#ifndef AUTOLOGIN_H
#define AUTOLOGIN_H
#include <QThread>
#include <QObject>
#include <QElapsedTimer>

class AutoLogin : public QThread
{
    Q_OBJECT
public:
    AutoLogin();

private:
    void run();
    void login();
    QElapsedTimer timer;

signals:
    void clickStartButton();
    void clickEnableAllButton();
    void clickPowerOffButton();
};

#endif // AUTOLOGIN_H
