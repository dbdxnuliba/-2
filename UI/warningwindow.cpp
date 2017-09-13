#include "warningwindow.h"
#include "ui_warningwindow.h"


void WarningWindow::setWarningText(QString text)
{
    ui->warningPanel->setText(text);
}

void WarningWindow::showCancelButton(bool status)
{
    ui->cancel->setVisible(status);
}

void WarningWindow::enableOKButton(bool status)
{
    if(status && !ui->ok->isEnabled()){
        on_ok_released();
    }

    ui->ok->setEnabled(status);
}

WarningWindow::WarningWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WarningWindow),
    yesOrNo(false),
    systemLanguage("cn")
{
    ui->setupUi(this);

    QPixmap bkgnd(":/images/background_number.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    ui->cancel->setVisible(false);

    //disable windows headers
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
}

WarningWindow::~WarningWindow()
{
    delete ui;
}

void WarningWindow::on_cancel_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/goBack_2.png");
        QIcon ButtonIcon(pixmap);
        ui->cancel->setIcon(ButtonIcon);
        ui->cancel->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/goBack_2.png");
        QIcon ButtonIcon(pixmap);
        ui->cancel->setIcon(ButtonIcon);
        ui->cancel->setIconSize(pixmap.rect().size());
    }
}

void WarningWindow::on_cancel_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/goBack.png");
        QIcon ButtonIcon(pixmap);
        ui->cancel->setIcon(ButtonIcon);
        ui->cancel->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/goBack.png");
        QIcon ButtonIcon(pixmap);
        ui->cancel->setIcon(ButtonIcon);
        ui->cancel->setIconSize(pixmap.rect().size());
    }
    yesOrNo = false;
    this->close();
}

void WarningWindow::on_ok_pressed()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/numOK_2.png");
        QIcon ButtonIcon(pixmap);
        ui->ok->setIcon(ButtonIcon);
        ui->ok->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/numOK_2.png");
        QIcon ButtonIcon(pixmap);
        ui->ok->setIcon(ButtonIcon);
        ui->ok->setIconSize(pixmap.rect().size());
    }
}

void WarningWindow::on_ok_released()
{
    if(systemLanguage == "cn"){
        QPixmap pixmap(":/images/numOK.png");
        QIcon ButtonIcon(pixmap);
        ui->ok->setIcon(ButtonIcon);
        ui->ok->setIconSize(pixmap.rect().size());
    }else if(systemLanguage == "en"){
        QPixmap pixmap(":/img_en/img_en/numOK.png");
        QIcon ButtonIcon(pixmap);
        ui->ok->setIcon(ButtonIcon);
        ui->ok->setIconSize(pixmap.rect().size());
    }
    yesOrNo = true;
    this->close();
}
