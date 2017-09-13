#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QDialog>

extern bool expertMode;

namespace Ui {
class Keyboard;
}

class Keyboard : public QDialog
{
    Q_OBJECT

private:
    Ui::Keyboard *ui;

signals:
    void sendText(int keyboardType, QString text);  //send text to MainWindow class

public:
    explicit Keyboard(QWidget *parent = 0);
    ~Keyboard();

    int keyboardType;                           //1:new program's name; 2:new function
    void cleanEditLine();                           //clean entered text
    QString getText();                              //get entered text
    void setText(QString text);                     //set text
    void expertModeSwitch(bool status);             //expert mode
    void on_enter_clicked_external();

private slots:
    void on_enter_clicked();

    void on_n1_clicked();

    void on_n2_clicked();

    void on_n3_clicked();

    void on_n4_clicked();

    void on_n5_clicked();

    void on_n6_clicked();

    void on_n7_clicked();

    void on_n8_clicked();

    void on_n9_clicked();

    void on_n0_clicked();

    void on_q_clicked();

    void on_w_clicked();

    void on_e_clicked();

    void on_r_clicked();

    void on_t_clicked();

    void on_y_clicked();

    void on_u_clicked();

    void on_i_clicked();

    void on_o_clicked();

    void on_p_clicked();

    void on_underline_clicked();

    void on_a_clicked();

    void on_s_clicked();

    void on_d_clicked();

    void on_f_clicked();

    void on_g_clicked();

    void on_h_clicked();

    void on_j_clicked();

    void on_k_clicked();

    void on_l_clicked();

    void on_parenthesesLeft_clicked();

    void on_parenthesesRight_clicked();

    void on_shift_clicked();

    void on_z_clicked();

    void on_x_clicked();

    void on_c_clicked();

    void on_v_clicked();

    void on_b_clicked();

    void on_n_clicked();

    void on_m_clicked();

    void on_point_clicked();

    void on_moreLess_clicked();

    void on_colon_clicked();

    void on_space_clicked();

    void on_backspace_pressed();

    void on_close_clicked();

    void on_n1_pressed();

    void on_n1_released();

    void on_n2_pressed();

    void on_n2_released();

    void on_n3_pressed();

    void on_n3_released();

    void on_n4_pressed();

    void on_n4_released();

    void on_n5_pressed();

    void on_n5_released();

    void on_n6_pressed();

    void on_n6_released();

    void on_n7_pressed();

    void on_n7_released();

    void on_n8_pressed();

    void on_n8_released();

    void on_n9_pressed();

    void on_n9_released();

    void on_n0_pressed();

    void on_n0_released();

    void on_q_pressed();

    void on_q_released();

    void on_w_pressed();

    void on_w_released();

    void on_e_pressed();

    void on_e_released();

    void on_r_pressed();

    void on_r_released();

    void on_t_pressed();

    void on_t_released();

    void on_y_pressed();

    void on_y_released();

    void on_u_pressed();

    void on_u_released();

    void on_i_pressed();

    void on_i_released();

    void on_o_pressed();

    void on_o_released();

    void on_p_pressed();

    void on_p_released();

    void on_a_pressed();

    void on_a_released();

    void on_s_pressed();

    void on_s_released();

    void on_d_pressed();

    void on_d_released();

    void on_f_pressed();

    void on_f_released();

    void on_g_pressed();

    void on_g_released();

    void on_h_pressed();

    void on_h_released();

    void on_j_pressed();

    void on_j_released();

    void on_k_pressed();

    void on_k_released();

    void on_l_pressed();

    void on_l_released();

    void on_z_pressed();

    void on_z_released();

    void on_x_pressed();

    void on_x_released();

    void on_c_pressed();

    void on_c_released();

    void on_v_pressed();

    void on_v_released();

    void on_b_pressed();

    void on_b_released();

    void on_n_pressed();

    void on_n_released();

    void on_m_pressed();

    void on_m_released();

    void on_backspace_released();

    void on_underline_pressed();

    void on_underline_released();

    void on_point_pressed();

    void on_point_released();

    void on_moreLess_pressed();

    void on_moreLess_released();

    void on_enter_pressed();

    void on_enter_released();

    void on_parenthesesLeft_pressed();

    void on_parenthesesLeft_released();

    void on_parenthesesRight_pressed();

    void on_parenthesesRight_released();

    void on_colon_pressed();

    void on_colon_released();

    void on_space_pressed();

    void on_space_released();

    void on_comma_clicked();

    void on_comma_pressed();

    void on_comma_released();

    void on_add_clicked();

    void on_add_pressed();

    void on_add_released();

    void on_tanhao_clicked();

    void on_tanhao_pressed();

    void on_tanhao_released();

    void on_midParenthese_clicked();

    void on_midParenthese_pressed();

    void on_midParenthese_released();
};

#endif // KEYBOARD_H
