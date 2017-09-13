#ifndef NUMBERBOARD_H
#define NUMBERBOARD_H

#include <QDialog>
#include "dataupdate.h"
#include "../threads/movetothread.h"

//for Chinese character
#pragma execution_character_set("utf-8")

namespace Ui {
class NumberBoard;
}

class NumberBoard : public QDialog
{
    Q_OBJECT

private:
    Ui::NumberBoard *ui;
    MoveToThread *moveToThread;

signals:
    void sendNumber(int whichOne, QString value);   //send entered number to MainWindow class
    void stopEnterValueMove();                      //when press to move button released, stop current move
    void startEnterValueMove();

public:
    explicit NumberBoard(QWidget *parent = 0);
    ~NumberBoard();

    void cleanLineEdit();                           //clear entered value
    void changeWindowSize();                        //change window size
    void updateValueOnAxisButtons();                //update current axis value on axis group box
    void updateValueOnPositionButtons();            //update current position value on position group box
    void enableDisableButtons(bool status);         //enable/disable number board buttons
    void setLabelText(QString text);                //change window title message
    void changeLanguage();                          //change language

    bool axisGroupVisible;                          //enter axis position value group box visible
    bool positionGroupVisible;                      //enter end position value group box visible
    int activeAxisNum;                              //current selected axis block on the window
    int activePositionNum;                          //current selected end position block on the window
    bool windowIsOpened;                            //an enter axis/end position value window is opened
    int editLineNum;                                //1:speed, 2:acceleration, 3:tool coordinate X,
                                                    //4:tool coordinate Y, 5:tool coordinate Z
                                                    //6:transition semidiameter
public slots:
    void on_pressToMove_pressed();

    void on_pressToMove_released();

private slots:
    void on_numberBoard0_clicked();

    void on_numberBoardPoint_clicked();

    void on_numberBoard1_clicked();

    void on_numberBoard2_clicked();

    void on_numberBoard3_clicked();

    void on_numberBoardOK_clicked();

    void on_numberBoard4_clicked();

    void on_numberBoard5_clicked();

    void on_numberBoard6_clicked();

    void on_numberBoard7_clicked();

    void on_numberBoard8_clicked();

    void on_numberBoard9_clicked();

    void on_numberBoardBack_clicked();

    void on_numberBoardClear_clicked();

    void on_numberBoardMinus_clicked();

    void on_axisLineEdit1_clicked();

    void on_axisLineEdit2_clicked();

    void on_axisLineEdit3_clicked();

    void on_axisLineEdit4_clicked();

    void on_axisLineEdit5_clicked();

    void on_axisLineEdit6_clicked();

    void on_axisLineEdit7_clicked();

    void on_xLineEdit_clicked();

    void on_yLineEdit_clicked();

    void on_zLineEdit_clicked();

    void on_rxLineEdit_clicked();

    void on_ryLineEdit_clicked();

    void on_rzLineEdit_clicked();

    void on_numberBoardCancel_clicked();

    void on_numberBoardBack_pressed();

    void on_numberBoardBack_released();

    void on_numberBoardClear_pressed();

    void on_numberBoardClear_released();

    void on_numberBoardCancel_pressed();

    void on_numberBoardCancel_released();

    void on_numberBoardOK_pressed();

    void on_numberBoardOK_released();

    void on_numberBoardPoint_pressed();

    void on_numberBoardPoint_released();

    void on_numberBoardMinus_pressed();

    void on_numberBoardMinus_released();

    void on_numberBoard0_pressed();

    void on_numberBoard0_released();

    void on_numberBoard1_pressed();

    void on_numberBoard1_released();

    void on_numberBoard2_pressed();

    void on_numberBoard2_released();

    void on_numberBoard3_pressed();

    void on_numberBoard3_released();

    void on_numberBoard4_pressed();

    void on_numberBoard4_released();

    void on_numberBoard5_pressed();

    void on_numberBoard5_released();

    void on_numberBoard6_pressed();

    void on_numberBoard6_released();

    void on_numberBoard7_pressed();

    void on_numberBoard7_released();

    void on_numberBoard8_pressed();

    void on_numberBoard8_released();

    void on_numberBoard9_pressed();

    void on_numberBoard9_released();
};

#endif // NUMBERBOARD_H
