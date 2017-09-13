#include "keyboard.h"
#include "ui_keyboard.h"
#include <QDebug>
#include <QThread>

bool upperCaseButton = false;
int cursorPos;

Keyboard::Keyboard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Keyboard),
    keyboardType(0)
{
    ui->setupUi(this);

    QPixmap bkgnd(":/images/background_keyboard.png");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    //disable windows headers
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

}

Keyboard::~Keyboard()
{
    delete ui;
}

void Keyboard::cleanEditLine()
{
    ui->keyboardLineEdit->setText("");
}

QString Keyboard::getText()
{
    return ui->keyboardLineEdit->text();
}

void Keyboard::setText(QString text)
{
    ui->keyboardLineEdit->setText(text);
}

void Keyboard::expertModeSwitch(bool status)
{
    status;
}

void Keyboard::on_enter_clicked_external()
{
    on_enter_clicked();
}

void Keyboard::on_n1_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "1"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);
}

void Keyboard::on_n2_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "2"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);
}

void Keyboard::on_n3_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "3"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);
}

void Keyboard::on_n4_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "4"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);
}

void Keyboard::on_n5_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "5"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}

void Keyboard::on_n6_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "6"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}

void Keyboard::on_n7_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "7"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}

void Keyboard::on_n8_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "8"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}

void Keyboard::on_n9_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "9"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}

void Keyboard::on_n0_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "0"));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}



void Keyboard::on_q_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "Q"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "q"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_w_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "W"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "w"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_e_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "E"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "e"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_r_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "R"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "r"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_t_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "T"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "t"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_y_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "Y"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "y"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_u_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "U"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "u"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_i_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "I"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "i"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_o_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "O"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "o"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_p_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "P"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "p"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_underline_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "_"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "-"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_a_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "A"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "a"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_s_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "S"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "s"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_d_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "D"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "d"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_f_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "F"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "f"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_g_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "G"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "g"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_h_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "H"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "h"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_j_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "J"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "j"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_k_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "K"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "k"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_l_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "L"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "l"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_parenthesesLeft_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "<"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "("));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_parenthesesRight_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, ">"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, ")"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_enter_clicked()
{

    if(ui->keyboardLineEdit->text() != ""){
        //for keyboardType == 2(function input), add ')' to the end of the function if user forget to add it
        //if(keyboardType == 2)
        //    if(ui->keyboardLineEdit->text().at(ui->keyboardLineEdit->text().count() - 1) != ')')
        //        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text() + ')');

        emit sendText(keyboardType, ui->keyboardLineEdit->text());

    }else{
        //modify robot script program
        if(keyboardType == 3) emit sendText(keyboardType, ui->keyboardLineEdit->text());

    }
}

void Keyboard::on_shift_clicked()
{
    upperCaseButton = !upperCaseButton;
    QPixmap pixmap;

    //change text
    if(upperCaseButton){
        //upper case
        QIcon ButtonIcon;
        pixmap.load(":/images/icon-shift_2.png");
        ButtonIcon.addPixmap(pixmap);
        ui->shift->setIcon(ButtonIcon);

        QIcon ButtonIcon2;
        pixmap.load(":/images/big_icon-Q.png");
        ButtonIcon2.addPixmap(pixmap);
        ui->q->setIcon(ButtonIcon2);

        QIcon ButtonIcon3;
        pixmap.load(":/images/big_icon-W.png");
        ButtonIcon3.addPixmap(pixmap);
        ui->w->setIcon(ButtonIcon3);

        QIcon ButtonIcon4;
        pixmap.load(":/images/big_icon-E.png");
        ButtonIcon4.addPixmap(pixmap);
        ui->e->setIcon(ButtonIcon4);

        QIcon ButtonIcon5;
        pixmap.load(":/images/big_icon-R.png");
        ButtonIcon5.addPixmap(pixmap);
        ui->r->setIcon(ButtonIcon5);

        QIcon ButtonIcon6;
        pixmap.load(":/images/big_icon-T.png");
        ButtonIcon6.addPixmap(pixmap);
        ui->t->setIcon(ButtonIcon6);

        QIcon ButtonIcon7;
        pixmap.load(":/images/big_icon-Y.png");
        ButtonIcon7.addPixmap(pixmap);
        ui->y->setIcon(ButtonIcon7);

        QIcon ButtonIcon8;
        pixmap.load(":/images/big_icon-U.png");
        ButtonIcon8.addPixmap(pixmap);
        ui->u->setIcon(ButtonIcon8);

        QIcon ButtonIcon9;
        pixmap.load(":/images/big_icon-I.png");
        ButtonIcon9.addPixmap(pixmap);
        ui->i->setIcon(ButtonIcon9);

        QIcon ButtonIcon10;
        pixmap.load(":/images/big_icon-O.png");
        ButtonIcon10.addPixmap(pixmap);
        ui->o->setIcon(ButtonIcon10);

        QIcon ButtonIcon11;
        pixmap.load(":/images/big_icon-P.png");
        ButtonIcon11.addPixmap(pixmap);
        ui->p->setIcon(ButtonIcon11);

        QIcon ButtonIcon12;
        pixmap.load(":/images/big_icon-A.png");
        ButtonIcon12.addPixmap(pixmap);
        ui->a->setIcon(ButtonIcon12);

        QIcon ButtonIcon13;
        pixmap.load(":/images/big_icon-S.png");
        ButtonIcon13.addPixmap(pixmap);
        ui->s->setIcon(ButtonIcon13);

        QIcon ButtonIcon14;
        pixmap.load(":/images/big_icon-D.png");
        ButtonIcon14.addPixmap(pixmap);
        ui->d->setIcon(ButtonIcon14);

        QIcon ButtonIcon15;
        pixmap.load(":/images/big_icon-F.png");
        ButtonIcon15.addPixmap(pixmap);
        ui->f->setIcon(ButtonIcon15);

        QIcon ButtonIcon16;
        pixmap.load(":/images/big_icon-G.png");
        ButtonIcon16.addPixmap(pixmap);
        ui->g->setIcon(ButtonIcon16);

        QIcon ButtonIcon17;
        pixmap.load(":/images/big_icon-H.png");
        ButtonIcon17.addPixmap(pixmap);
        ui->h->setIcon(ButtonIcon17);

        QIcon ButtonIcon18;
        pixmap.load(":/images/big_icon-J.png");
        ButtonIcon18.addPixmap(pixmap);
        ui->j->setIcon(ButtonIcon18);

        QIcon ButtonIcon19;
        pixmap.load(":/images/big_icon-K.png");
        ButtonIcon19.addPixmap(pixmap);
        ui->k->setIcon(ButtonIcon19);

        QIcon ButtonIcon20;
        pixmap.load(":/images/big_icon-L.png");
        ButtonIcon20.addPixmap(pixmap);
        ui->l->setIcon(ButtonIcon20);

        QIcon ButtonIcon21;
        pixmap.load(":/images/big_icon-Z.png");
        ButtonIcon21.addPixmap(pixmap);
        ui->z->setIcon(ButtonIcon21);

        QIcon ButtonIcon22;
        pixmap.load(":/images/big_icon-X.png");
        ButtonIcon22.addPixmap(pixmap);
        ui->x->setIcon(ButtonIcon22);

        QIcon ButtonIcon23;
        pixmap.load(":/images/big_icon-C.png");
        ButtonIcon23.addPixmap(pixmap);
        ui->c->setIcon(ButtonIcon23);

        QIcon ButtonIcon24;
        pixmap.load(":/images/big_icon-V.png");
        ButtonIcon24.addPixmap(pixmap);
        ui->v->setIcon(ButtonIcon24);

        QIcon ButtonIcon25;
        pixmap.load(":/images/big_icon-B.png");
        ButtonIcon25.addPixmap(pixmap);
        ui->b->setIcon(ButtonIcon25);

        QIcon ButtonIcon26;
        pixmap.load(":/images/big_icon-N.png");
        ButtonIcon26.addPixmap(pixmap);
        ui->n->setIcon(ButtonIcon26);

        QIcon ButtonIcon27;
        pixmap.load(":/images/big_icon-M.png");
        ButtonIcon27.addPixmap(pixmap);
        ui->m->setIcon(ButtonIcon27);





        QIcon ButtonIcon28;
        pixmap.load(":/images/icon-divide.png");
        ButtonIcon28.addPixmap(pixmap);
        ui->point->setIcon(ButtonIcon28);

        QIcon ButtonIcon29;
        pixmap.load(":/images/icon-underLine.png");
        ButtonIcon29.addPixmap(pixmap);
        ui->underline->setIcon(ButtonIcon29);

        QIcon ButtonIcon30;
        pixmap.load(":/images/icon-2dot.png");
        ButtonIcon30.addPixmap(pixmap);
        ui->colon->setIcon(ButtonIcon30);

        QIcon ButtonIcon31;
        pixmap.load(":/images/icon-greater.png");
        ButtonIcon31.addPixmap(pixmap);
        ui->parenthesesRight->setIcon(ButtonIcon31);

        QIcon ButtonIcon32;
        pixmap.load(":/images/icon-less.png");
        ButtonIcon32.addPixmap(pixmap);
        ui->parenthesesLeft->setIcon(ButtonIcon32);

        QIcon ButtonIcon33;
        pixmap.load(":/images/icon-quotation.png");
        ButtonIcon33.addPixmap(pixmap);
        ui->moreLess->setIcon(ButtonIcon33);

        QIcon ButtonIcon34;
        pixmap.load(":/images/icon-mul.png");
        ButtonIcon34.addPixmap(pixmap);
        ui->comma->setIcon(ButtonIcon34);

        QIcon ButtonIcon35;
        pixmap.load(":/images/icon-#.png");
        ButtonIcon35.addPixmap(pixmap);
        ui->add->setIcon(ButtonIcon35);

        QIcon ButtonIcon36;
        pixmap.load(":/images/icon-{.png");
        ButtonIcon36.addPixmap(pixmap);
        ui->tanhao->setIcon(ButtonIcon36);

        QIcon ButtonIcon37;
        pixmap.load(":/images/icon-].png");
        ButtonIcon37.addPixmap(pixmap);
        ui->midParenthese->setIcon(ButtonIcon37);
    }else{
        //lower case
        QIcon ButtonIcon;
        pixmap.load(":/images/icon-shift.png");
        ButtonIcon.addPixmap(pixmap);
        ui->shift->setIcon(ButtonIcon);

        QIcon ButtonIcon1;
        pixmap.load(":/images/icon-q.png");
        ButtonIcon1.addPixmap(pixmap);
        ui->q->setIcon(ButtonIcon1);

        QIcon ButtonIcon2;
        pixmap.load(":/images/icon-w.png");
        ButtonIcon2.addPixmap(pixmap);
        ui->w->setIcon(ButtonIcon2);

        QIcon ButtonIcon3;
        pixmap.load(":/images/icon-e.png");
        ButtonIcon3.addPixmap(pixmap);
        ui->e->setIcon(ButtonIcon3);

        QIcon ButtonIcon4;
        pixmap.load(":/images/icon-r.png");
        ButtonIcon4.addPixmap(pixmap);
        ui->r->setIcon(ButtonIcon4);

        QIcon ButtonIcon5;
        pixmap.load(":/images/icon-t.png");
        ButtonIcon5.addPixmap(pixmap);
        ui->t->setIcon(ButtonIcon5);

        QIcon ButtonIcon6;
        pixmap.load(":/images/icon-y.png");
        ButtonIcon6.addPixmap(pixmap);
        ui->y->setIcon(ButtonIcon6);

        QIcon ButtonIcon7;
        pixmap.load(":/images/icon-u.png");
        ButtonIcon7.addPixmap(pixmap);
        ui->u->setIcon(ButtonIcon7);

        QIcon ButtonIcon8;
        pixmap.load(":/images/icon-i.png");
        ButtonIcon8.addPixmap(pixmap);
        ui->i->setIcon(ButtonIcon8);

        QIcon ButtonIcon9;
        pixmap.load(":/images/icon-o.png");
        ButtonIcon9.addPixmap(pixmap);
        ui->o->setIcon(ButtonIcon9);

        QIcon ButtonIcon10;
        pixmap.load(":/images/icon-p.png");
        ButtonIcon10.addPixmap(pixmap);
        ui->p->setIcon(ButtonIcon10);

        QIcon ButtonIcon11;
        pixmap.load(":/images/icon-a.png");
        ButtonIcon11.addPixmap(pixmap);
        ui->a->setIcon(ButtonIcon11);

        QIcon ButtonIcon12;
        pixmap.load(":/images/icon-s.png");
        ButtonIcon12.addPixmap(pixmap);
        ui->s->setIcon(ButtonIcon12);

        QIcon ButtonIcon13;
        pixmap.load(":/images/icon-d.png");
        ButtonIcon13.addPixmap(pixmap);
        ui->d->setIcon(ButtonIcon13);

        QIcon ButtonIcon14;
        pixmap.load(":/images/icon-f.png");
        ButtonIcon14.addPixmap(pixmap);
        ui->f->setIcon(ButtonIcon14);

        QIcon ButtonIcon15;
        pixmap.load(":/images/icon-g.png");
        ButtonIcon15.addPixmap(pixmap);
        ui->g->setIcon(ButtonIcon15);

        QIcon ButtonIcon16;
        pixmap.load(":/images/icon-h.png");
        ButtonIcon16.addPixmap(pixmap);
        ui->h->setIcon(ButtonIcon16);

        QIcon ButtonIcon17;
        pixmap.load(":/images/icon-j.png");
        ButtonIcon17.addPixmap(pixmap);
        ui->j->setIcon(ButtonIcon17);

        QIcon ButtonIcon18;
        pixmap.load(":/images/icon-k.png");
        ButtonIcon18.addPixmap(pixmap);
        ui->k->setIcon(ButtonIcon18);

        QIcon ButtonIcon19;
        pixmap.load(":/images/icon-l.png");
        ButtonIcon19.addPixmap(pixmap);
        ui->l->setIcon(ButtonIcon19);

        QIcon ButtonIcon20;
        pixmap.load(":/images/icon-z.png");
        ButtonIcon20.addPixmap(pixmap);
        ui->z->setIcon(ButtonIcon20);

        QIcon ButtonIcon21;
        pixmap.load(":/images/icon-x.png");
        ButtonIcon21.addPixmap(pixmap);
        ui->x->setIcon(ButtonIcon21);

        QIcon ButtonIcon22;
        pixmap.load(":/images/icon-c.png");
        ButtonIcon22.addPixmap(pixmap);
        ui->c->setIcon(ButtonIcon22);

        QIcon ButtonIcon23;
        pixmap.load(":/images/icon-v.png");
        ButtonIcon23.addPixmap(pixmap);
        ui->v->setIcon(ButtonIcon23);

        QIcon ButtonIcon24;
        pixmap.load(":/images/icon-b.png");
        ButtonIcon24.addPixmap(pixmap);
        ui->b->setIcon(ButtonIcon24);

        QIcon ButtonIcon25;
        pixmap.load(":/images/icon-n.png");
        ButtonIcon25.addPixmap(pixmap);
        ui->n->setIcon(ButtonIcon25);

        QIcon ButtonIcon26;
        pixmap.load(":/images/icon-m.png");
        ButtonIcon26.addPixmap(pixmap);
        ui->m->setIcon(ButtonIcon26);




        QIcon ButtonIcon28;
        pixmap.load(":/images/icon-comma.png");
        ButtonIcon28.addPixmap(pixmap);
        ui->point->setIcon(ButtonIcon28);

        QIcon ButtonIcon29;
        pixmap.load(":/images/icon-minus.png");
        ButtonIcon29.addPixmap(pixmap);
        ui->underline->setIcon(ButtonIcon29);

        QIcon ButtonIcon30;
        pixmap.load(":/images/icon-equal.png");
        ButtonIcon30.addPixmap(pixmap);
        ui->colon->setIcon(ButtonIcon30);

        QIcon ButtonIcon31;
        pixmap.load(":/images/icon-right.png");
        ButtonIcon31.addPixmap(pixmap);
        ui->parenthesesRight->setIcon(ButtonIcon31);

        QIcon ButtonIcon32;
        pixmap.load(":/images/icon-left.png");
        ButtonIcon32.addPixmap(pixmap);
        ui->parenthesesLeft->setIcon(ButtonIcon32);

        QIcon ButtonIcon33;
        pixmap.load(":/images/icon-dot.png");
        ButtonIcon33.addPixmap(pixmap);
        ui->moreLess->setIcon(ButtonIcon33);

        QIcon ButtonIcon34;
        pixmap.load(":/images/icon-,.png");
        ButtonIcon34.addPixmap(pixmap);
        ui->comma->setIcon(ButtonIcon34);

        QIcon ButtonIcon35;
        pixmap.load(":/images/icon-+.png");
        ButtonIcon35.addPixmap(pixmap);
        ui->add->setIcon(ButtonIcon35);

        QIcon ButtonIcon36;
        pixmap.load(":/images/icon-!.png");
        ButtonIcon36.addPixmap(pixmap);
        ui->tanhao->setIcon(ButtonIcon36);

        QIcon ButtonIcon37;
        pixmap.load(":/images/icon-[.png");
        ButtonIcon37.addPixmap(pixmap);
        ui->midParenthese->setIcon(ButtonIcon37);
    }
}

void Keyboard::on_z_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "Z"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "z"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_x_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "X"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "x"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_c_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "C"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "c"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_v_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "V"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "v"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_b_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "B"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "b"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_n_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "N"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "n"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_m_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "M"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "m"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_point_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "/"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, ";"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_moreLess_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "\""));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);  }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "."));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_colon_clicked()
{
    if(upperCaseButton){
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, ":"));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1); }
    else{
        cursorPos = ui->keyboardLineEdit->cursorPosition();
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "="));
        ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);}
}

void Keyboard::on_space_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, " "));
    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);
}

void Keyboard::on_close_clicked()
{
    this->close(); //close the keyboard
}

void Keyboard::on_n1_pressed()
{
    QPixmap pixmap(":/images/icon-1_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n1->setIcon(ButtonIcon);
    ui->n1->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n1_released()
{
    QPixmap pixmap(":/images/icon-1.png");
    QIcon ButtonIcon(pixmap);
    ui->n1->setIcon(ButtonIcon);
    ui->n1->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n2_pressed()
{
    QPixmap pixmap(":/images/icon-2_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n2->setIcon(ButtonIcon);
    ui->n2->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n2_released()
{
    QPixmap pixmap(":/images/icon-2.png");
    QIcon ButtonIcon(pixmap);
    ui->n2->setIcon(ButtonIcon);
    ui->n2->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n3_pressed()
{
    QPixmap pixmap(":/images/icon-3_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n3->setIcon(ButtonIcon);
    ui->n3->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n3_released()
{
    QPixmap pixmap(":/images/icon-3.png");
    QIcon ButtonIcon(pixmap);
    ui->n3->setIcon(ButtonIcon);
    ui->n3->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n4_pressed()
{
    QPixmap pixmap(":/images/icon-4_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n4->setIcon(ButtonIcon);
    ui->n4->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n4_released()
{
    QPixmap pixmap(":/images/icon-4.png");
    QIcon ButtonIcon(pixmap);
    ui->n4->setIcon(ButtonIcon);
    ui->n4->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n5_pressed()
{
    QPixmap pixmap(":/images/icon-5_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n5->setIcon(ButtonIcon);
    ui->n5->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n5_released()
{
    QPixmap pixmap(":/images/icon-5.png");
    QIcon ButtonIcon(pixmap);
    ui->n5->setIcon(ButtonIcon);
    ui->n5->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n6_pressed()
{
    QPixmap pixmap(":/images/icon-6_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n6->setIcon(ButtonIcon);
    ui->n6->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n6_released()
{
    QPixmap pixmap(":/images/icon-6.png");
    QIcon ButtonIcon(pixmap);
    ui->n6->setIcon(ButtonIcon);
    ui->n6->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n7_pressed()
{
    QPixmap pixmap(":/images/icon-7_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n7->setIcon(ButtonIcon);
    ui->n7->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n7_released()
{
    QPixmap pixmap(":/images/icon-7.png");
    QIcon ButtonIcon(pixmap);
    ui->n7->setIcon(ButtonIcon);
    ui->n7->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n8_pressed()
{
    QPixmap pixmap(":/images/icon-8_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n8->setIcon(ButtonIcon);
    ui->n8->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n8_released()
{
    QPixmap pixmap(":/images/icon-8.png");
    QIcon ButtonIcon(pixmap);
    ui->n8->setIcon(ButtonIcon);
    ui->n8->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n9_pressed()
{
    QPixmap pixmap(":/images/icon-9_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n9->setIcon(ButtonIcon);
    ui->n9->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n9_released()
{
    QPixmap pixmap(":/images/icon-9.png");
    QIcon ButtonIcon(pixmap);
    ui->n9->setIcon(ButtonIcon);
    ui->n9->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n0_pressed()
{
    QPixmap pixmap(":/images/icon-0_2.png");
    QIcon ButtonIcon(pixmap);
    ui->n0->setIcon(ButtonIcon);
    ui->n0->setIconSize(pixmap.rect().size());
}

void Keyboard::on_n0_released()
{
    QPixmap pixmap(":/images/icon-0.png");
    QIcon ButtonIcon(pixmap);
    ui->n0->setIcon(ButtonIcon);
    ui->n0->setIconSize(pixmap.rect().size());
}

void Keyboard::on_q_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-Q_2.png");
        QIcon ButtonIcon(pixmap);
        ui->q->setIcon(ButtonIcon);
        ui->q->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-q_2.png");
        QIcon ButtonIcon(pixmap);
        ui->q->setIcon(ButtonIcon);
        ui->q->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_q_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-Q.png");
        QIcon ButtonIcon(pixmap);
        ui->q->setIcon(ButtonIcon);
        ui->q->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-q.png");
        QIcon ButtonIcon(pixmap);
        ui->q->setIcon(ButtonIcon);
        ui->q->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_w_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-W_2.png");
        QIcon ButtonIcon(pixmap);
        ui->w->setIcon(ButtonIcon);
        ui->w->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-w_2.png");
        QIcon ButtonIcon(pixmap);
        ui->w->setIcon(ButtonIcon);
        ui->w->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_w_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-W.png");
        QIcon ButtonIcon(pixmap);
        ui->w->setIcon(ButtonIcon);
        ui->w->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-w.png");
        QIcon ButtonIcon(pixmap);
        ui->w->setIcon(ButtonIcon);
        ui->w->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_e_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-E_2.png");
        QIcon ButtonIcon(pixmap);
        ui->e->setIcon(ButtonIcon);
        ui->e->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-e_2.png");
        QIcon ButtonIcon(pixmap);
        ui->e->setIcon(ButtonIcon);
        ui->e->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_e_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-E.png");
        QIcon ButtonIcon(pixmap);
        ui->e->setIcon(ButtonIcon);
        ui->e->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-e.png");
        QIcon ButtonIcon(pixmap);
        ui->e->setIcon(ButtonIcon);
        ui->e->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_r_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-R_2.png");
        QIcon ButtonIcon(pixmap);
        ui->r->setIcon(ButtonIcon);
        ui->r->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-r_2.png");
        QIcon ButtonIcon(pixmap);
        ui->r->setIcon(ButtonIcon);
        ui->r->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_r_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-R.png");
        QIcon ButtonIcon(pixmap);
        ui->r->setIcon(ButtonIcon);
        ui->r->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-r.png");
        QIcon ButtonIcon(pixmap);
        ui->r->setIcon(ButtonIcon);
        ui->r->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_t_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-T_2.png");
        QIcon ButtonIcon(pixmap);
        ui->t->setIcon(ButtonIcon);
        ui->t->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-t_2.png");
        QIcon ButtonIcon(pixmap);
        ui->t->setIcon(ButtonIcon);
        ui->t->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_t_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-T.png");
        QIcon ButtonIcon(pixmap);
        ui->t->setIcon(ButtonIcon);
        ui->t->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-t.png");
        QIcon ButtonIcon(pixmap);
        ui->t->setIcon(ButtonIcon);
        ui->t->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_y_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-Y_2.png");
        QIcon ButtonIcon(pixmap);
        ui->y->setIcon(ButtonIcon);
        ui->y->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-y_2.png");
        QIcon ButtonIcon(pixmap);
        ui->y->setIcon(ButtonIcon);
        ui->y->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_y_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-Y.png");
        QIcon ButtonIcon(pixmap);
        ui->y->setIcon(ButtonIcon);
        ui->y->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-y.png");
        QIcon ButtonIcon(pixmap);
        ui->y->setIcon(ButtonIcon);
        ui->y->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_u_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-U_2.png");
        QIcon ButtonIcon(pixmap);
        ui->u->setIcon(ButtonIcon);
        ui->u->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-u_2.png");
        QIcon ButtonIcon(pixmap);
        ui->u->setIcon(ButtonIcon);
        ui->u->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_u_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-U.png");
        QIcon ButtonIcon(pixmap);
        ui->u->setIcon(ButtonIcon);
        ui->u->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-u.png");
        QIcon ButtonIcon(pixmap);
        ui->u->setIcon(ButtonIcon);
        ui->u->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_i_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-I_2.png");
        QIcon ButtonIcon(pixmap);
        ui->i->setIcon(ButtonIcon);
        ui->i->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-i_2.png");
        QIcon ButtonIcon(pixmap);
        ui->i->setIcon(ButtonIcon);
        ui->i->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_i_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-I.png");
        QIcon ButtonIcon(pixmap);
        ui->i->setIcon(ButtonIcon);
        ui->i->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-i.png");
        QIcon ButtonIcon(pixmap);
        ui->i->setIcon(ButtonIcon);
        ui->i->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_o_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-O_2.png");
        QIcon ButtonIcon(pixmap);
        ui->o->setIcon(ButtonIcon);
        ui->o->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-o_2.png");
        QIcon ButtonIcon(pixmap);
        ui->o->setIcon(ButtonIcon);
        ui->o->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_o_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-O.png");
        QIcon ButtonIcon(pixmap);
        ui->o->setIcon(ButtonIcon);
        ui->o->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-o.png");
        QIcon ButtonIcon(pixmap);
        ui->o->setIcon(ButtonIcon);
        ui->o->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_p_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-P_2.png");
        QIcon ButtonIcon(pixmap);
        ui->p->setIcon(ButtonIcon);
        ui->p->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-p_2.png");
        QIcon ButtonIcon(pixmap);
        ui->p->setIcon(ButtonIcon);
        ui->p->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_p_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-P.png");
        QIcon ButtonIcon(pixmap);
        ui->p->setIcon(ButtonIcon);
        ui->p->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-p.png");
        QIcon ButtonIcon(pixmap);
        ui->p->setIcon(ButtonIcon);
        ui->p->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_a_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-A_2.png");
        QIcon ButtonIcon(pixmap);
        ui->a->setIcon(ButtonIcon);
        ui->a->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-a_2.png");
        QIcon ButtonIcon(pixmap);
        ui->a->setIcon(ButtonIcon);
        ui->a->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_a_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-A.png");
        QIcon ButtonIcon(pixmap);
        ui->a->setIcon(ButtonIcon);
        ui->a->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-a.png");
        QIcon ButtonIcon(pixmap);
        ui->a->setIcon(ButtonIcon);
        ui->a->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_s_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-S_2.png");
        QIcon ButtonIcon(pixmap);
        ui->s->setIcon(ButtonIcon);
        ui->s->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-s_2.png");
        QIcon ButtonIcon(pixmap);
        ui->s->setIcon(ButtonIcon);
        ui->s->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_s_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-S.png");
        QIcon ButtonIcon(pixmap);
        ui->s->setIcon(ButtonIcon);
        ui->s->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-s.png");
        QIcon ButtonIcon(pixmap);
        ui->s->setIcon(ButtonIcon);
        ui->s->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_d_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-D_2.png");
        QIcon ButtonIcon(pixmap);
        ui->d->setIcon(ButtonIcon);
        ui->d->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-d_2.png");
        QIcon ButtonIcon(pixmap);
        ui->d->setIcon(ButtonIcon);
        ui->d->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_d_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-D.png");
        QIcon ButtonIcon(pixmap);
        ui->d->setIcon(ButtonIcon);
        ui->d->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-d.png");
        QIcon ButtonIcon(pixmap);
        ui->d->setIcon(ButtonIcon);
        ui->d->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_f_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-F_2.png");
        QIcon ButtonIcon(pixmap);
        ui->f->setIcon(ButtonIcon);
        ui->f->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-f_2.png");
        QIcon ButtonIcon(pixmap);
        ui->f->setIcon(ButtonIcon);
        ui->f->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_f_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-F.png");
        QIcon ButtonIcon(pixmap);
        ui->f->setIcon(ButtonIcon);
        ui->f->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-f.png");
        QIcon ButtonIcon(pixmap);
        ui->f->setIcon(ButtonIcon);
        ui->f->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_g_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-G_2.png");
        QIcon ButtonIcon(pixmap);
        ui->g->setIcon(ButtonIcon);
        ui->g->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-g_2.png");
        QIcon ButtonIcon(pixmap);
        ui->g->setIcon(ButtonIcon);
        ui->g->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_g_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-G.png");
        QIcon ButtonIcon(pixmap);
        ui->g->setIcon(ButtonIcon);
        ui->g->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-g.png");
        QIcon ButtonIcon(pixmap);
        ui->g->setIcon(ButtonIcon);
        ui->g->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_h_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-H_2.png");
        QIcon ButtonIcon(pixmap);
        ui->h->setIcon(ButtonIcon);
        ui->h->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-h_2.png");
        QIcon ButtonIcon(pixmap);
        ui->h->setIcon(ButtonIcon);
        ui->h->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_h_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-H.png");
        QIcon ButtonIcon(pixmap);
        ui->h->setIcon(ButtonIcon);
        ui->h->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-h.png");
        QIcon ButtonIcon(pixmap);
        ui->h->setIcon(ButtonIcon);
        ui->h->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_j_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-J_2.png");
        QIcon ButtonIcon(pixmap);
        ui->j->setIcon(ButtonIcon);
        ui->j->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-j_2.png");
        QIcon ButtonIcon(pixmap);
        ui->j->setIcon(ButtonIcon);
        ui->j->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_j_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-J.png");
        QIcon ButtonIcon(pixmap);
        ui->j->setIcon(ButtonIcon);
        ui->j->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-j.png");
        QIcon ButtonIcon(pixmap);
        ui->j->setIcon(ButtonIcon);
        ui->j->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_k_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-K_2.png");
        QIcon ButtonIcon(pixmap);
        ui->k->setIcon(ButtonIcon);
        ui->k->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-k_2.png");
        QIcon ButtonIcon(pixmap);
        ui->k->setIcon(ButtonIcon);
        ui->k->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_k_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-K.png");
        QIcon ButtonIcon(pixmap);
        ui->k->setIcon(ButtonIcon);
        ui->k->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-k.png");
        QIcon ButtonIcon(pixmap);
        ui->k->setIcon(ButtonIcon);
        ui->k->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_l_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-L_2.png");
        QIcon ButtonIcon(pixmap);
        ui->l->setIcon(ButtonIcon);
        ui->l->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-l_2.png");
        QIcon ButtonIcon(pixmap);
        ui->l->setIcon(ButtonIcon);
        ui->l->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_l_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-L.png");
        QIcon ButtonIcon(pixmap);
        ui->l->setIcon(ButtonIcon);
        ui->l->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-l.png");
        QIcon ButtonIcon(pixmap);
        ui->l->setIcon(ButtonIcon);
        ui->l->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_z_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-Z_2.png");
        QIcon ButtonIcon(pixmap);
        ui->z->setIcon(ButtonIcon);
        ui->z->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-z_2.png");
        QIcon ButtonIcon(pixmap);
        ui->z->setIcon(ButtonIcon);
        ui->z->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_z_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-Z.png");
        QIcon ButtonIcon(pixmap);
        ui->z->setIcon(ButtonIcon);
        ui->z->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-z.png");
        QIcon ButtonIcon(pixmap);
        ui->z->setIcon(ButtonIcon);
        ui->z->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_x_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-X_2.png");
        QIcon ButtonIcon(pixmap);
        ui->x->setIcon(ButtonIcon);
        ui->x->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-x_2.png");
        QIcon ButtonIcon(pixmap);
        ui->x->setIcon(ButtonIcon);
        ui->x->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_x_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-X.png");
        QIcon ButtonIcon(pixmap);
        ui->x->setIcon(ButtonIcon);
        ui->x->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-x.png");
        QIcon ButtonIcon(pixmap);
        ui->x->setIcon(ButtonIcon);
        ui->x->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_c_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-C_2.png");
        QIcon ButtonIcon(pixmap);
        ui->c->setIcon(ButtonIcon);
        ui->c->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-c_2.png");
        QIcon ButtonIcon(pixmap);
        ui->c->setIcon(ButtonIcon);
        ui->c->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_c_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-C.png");
        QIcon ButtonIcon(pixmap);
        ui->c->setIcon(ButtonIcon);
        ui->c->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-c.png");
        QIcon ButtonIcon(pixmap);
        ui->c->setIcon(ButtonIcon);
        ui->c->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_v_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-V_2.png");
        QIcon ButtonIcon(pixmap);
        ui->v->setIcon(ButtonIcon);
        ui->v->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-v_2.png");
        QIcon ButtonIcon(pixmap);
        ui->v->setIcon(ButtonIcon);
        ui->v->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_v_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-V.png");
        QIcon ButtonIcon(pixmap);
        ui->v->setIcon(ButtonIcon);
        ui->v->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-v.png");
        QIcon ButtonIcon(pixmap);
        ui->v->setIcon(ButtonIcon);
        ui->v->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_b_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-B_2.png");
        QIcon ButtonIcon(pixmap);
        ui->b->setIcon(ButtonIcon);
        ui->b->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-b_2.png");
        QIcon ButtonIcon(pixmap);
        ui->b->setIcon(ButtonIcon);
        ui->b->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_b_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-B.png");
        QIcon ButtonIcon(pixmap);
        ui->b->setIcon(ButtonIcon);
        ui->b->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-b.png");
        QIcon ButtonIcon(pixmap);
        ui->b->setIcon(ButtonIcon);
        ui->b->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_n_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-N_2.png");
        QIcon ButtonIcon(pixmap);
        ui->n->setIcon(ButtonIcon);
        ui->n->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-n_2.png");
        QIcon ButtonIcon(pixmap);
        ui->n->setIcon(ButtonIcon);
        ui->n->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_n_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-N.png");
        QIcon ButtonIcon(pixmap);
        ui->n->setIcon(ButtonIcon);
        ui->n->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-n.png");
        QIcon ButtonIcon(pixmap);
        ui->n->setIcon(ButtonIcon);
        ui->n->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_m_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-M_2.png");
        QIcon ButtonIcon(pixmap);
        ui->m->setIcon(ButtonIcon);
        ui->m->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-m_2.png");
        QIcon ButtonIcon(pixmap);
        ui->m->setIcon(ButtonIcon);
        ui->m->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_m_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/big_icon-M.png");
        QIcon ButtonIcon(pixmap);
        ui->m->setIcon(ButtonIcon);
        ui->m->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-m.png");
        QIcon ButtonIcon(pixmap);
        ui->m->setIcon(ButtonIcon);
        ui->m->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_backspace_pressed()
{
    int currentCursor = ui->keyboardLineEdit->cursorPosition();
    ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().remove(currentCursor - 1, 1));
    ui->keyboardLineEdit->setCursorPosition(currentCursor - 1);


    QPixmap pixmap(":/images/icon-cancel_2.png");
    QIcon ButtonIcon(pixmap);
    ui->backspace->setIcon(ButtonIcon);
    ui->backspace->setIconSize(pixmap.rect().size());
}

void Keyboard::on_backspace_released()
{
    QPixmap pixmap(":/images/icon-cancel.png");
    QIcon ButtonIcon(pixmap);
    ui->backspace->setIcon(ButtonIcon);
    ui->backspace->setIconSize(pixmap.rect().size());
}

void Keyboard::on_underline_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-underLine_2.png");
        QIcon ButtonIcon(pixmap);
        ui->underline->setIcon(ButtonIcon);
        ui->underline->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-minus_2.png");
        QIcon ButtonIcon(pixmap);
        ui->underline->setIcon(ButtonIcon);
        ui->underline->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_underline_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-underLine.png");
        QIcon ButtonIcon(pixmap);
        ui->underline->setIcon(ButtonIcon);
        ui->underline->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-minus.png");
        QIcon ButtonIcon(pixmap);
        ui->underline->setIcon(ButtonIcon);
        ui->underline->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_point_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-divide_2.png");
        QIcon ButtonIcon(pixmap);
        ui->point->setIcon(ButtonIcon);
        ui->point->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-comma_2.png");
        QIcon ButtonIcon(pixmap);
        ui->point->setIcon(ButtonIcon);
        ui->point->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_point_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-divide.png");
        QIcon ButtonIcon(pixmap);
        ui->point->setIcon(ButtonIcon);
        ui->point->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-comma.png");
        QIcon ButtonIcon(pixmap);
        ui->point->setIcon(ButtonIcon);
        ui->point->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_moreLess_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-quotation_2.png");
        QIcon ButtonIcon(pixmap);
        ui->moreLess->setIcon(ButtonIcon);
        ui->moreLess->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-dot_2.png");
        QIcon ButtonIcon(pixmap);
        ui->moreLess->setIcon(ButtonIcon);
        ui->moreLess->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_moreLess_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-quotation.png");
        QIcon ButtonIcon(pixmap);
        ui->moreLess->setIcon(ButtonIcon);
        ui->moreLess->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-dot.png");
        QIcon ButtonIcon(pixmap);
        ui->moreLess->setIcon(ButtonIcon);
        ui->moreLess->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_enter_pressed()
{
    QPixmap pixmap(":/images/icon-enter_2.png");
    QIcon ButtonIcon(pixmap);
    ui->enter->setIcon(ButtonIcon);
    ui->enter->setIconSize(pixmap.rect().size());
}

void Keyboard::on_enter_released()
{
    QPixmap pixmap(":/images/icon-enter.png");
    QIcon ButtonIcon(pixmap);
    ui->enter->setIcon(ButtonIcon);
    ui->enter->setIconSize(pixmap.rect().size());
}

void Keyboard::on_parenthesesLeft_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-less_2.png");
        QIcon ButtonIcon(pixmap);
        ui->parenthesesLeft->setIcon(ButtonIcon);
        ui->parenthesesLeft->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-left_2.png");
        QIcon ButtonIcon(pixmap);
        ui->parenthesesLeft->setIcon(ButtonIcon);
        ui->parenthesesLeft->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_parenthesesLeft_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-less.png");
        QIcon ButtonIcon(pixmap);
        ui->parenthesesLeft->setIcon(ButtonIcon);
        ui->parenthesesLeft->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-left.png");
        QIcon ButtonIcon(pixmap);
        ui->parenthesesLeft->setIcon(ButtonIcon);
        ui->parenthesesLeft->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_parenthesesRight_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-greater_2.png");
        QIcon ButtonIcon(pixmap);
        ui->parenthesesRight->setIcon(ButtonIcon);
        ui->parenthesesRight->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-right_2.png");
        QIcon ButtonIcon(pixmap);
        ui->parenthesesRight->setIcon(ButtonIcon);
        ui->parenthesesRight->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_parenthesesRight_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-greater.png");
        QIcon ButtonIcon(pixmap);
        ui->parenthesesRight->setIcon(ButtonIcon);
        ui->parenthesesRight->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-right.png");
        QIcon ButtonIcon(pixmap);
        ui->parenthesesRight->setIcon(ButtonIcon);
        ui->parenthesesRight->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_colon_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-2dot_2.png");
        QIcon ButtonIcon(pixmap);
        ui->colon->setIcon(ButtonIcon);
        ui->colon->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-equal_2.png");
        QIcon ButtonIcon(pixmap);
        ui->colon->setIcon(ButtonIcon);
        ui->colon->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_colon_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-2dot.png");
        QIcon ButtonIcon(pixmap);
        ui->colon->setIcon(ButtonIcon);
        ui->colon->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-equal.png");
        QIcon ButtonIcon(pixmap);
        ui->colon->setIcon(ButtonIcon);
        ui->colon->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_space_pressed()
{
    QPixmap pixmap(":/images/icon-space_2.png");
    QIcon ButtonIcon(pixmap);
    ui->space->setIcon(ButtonIcon);
    ui->space->setIconSize(pixmap.rect().size());
}

void Keyboard::on_space_released()
{
    QPixmap pixmap(":/images/icon-space.png");
    QIcon ButtonIcon(pixmap);
    ui->space->setIcon(ButtonIcon);
    ui->space->setIconSize(pixmap.rect().size());
}

void Keyboard::on_comma_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();

    if(upperCaseButton)
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "*"));
    else
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, ","));

    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);
}

void Keyboard::on_comma_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-mul_2.png");
        QIcon ButtonIcon(pixmap);
        ui->comma->setIcon(ButtonIcon);
        ui->comma->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-,_2.png");
        QIcon ButtonIcon(pixmap);
        ui->comma->setIcon(ButtonIcon);
        ui->comma->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_comma_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-mul.png");
        QIcon ButtonIcon(pixmap);
        ui->comma->setIcon(ButtonIcon);
        ui->comma->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-,.png");
        QIcon ButtonIcon(pixmap);
        ui->comma->setIcon(ButtonIcon);
        ui->comma->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_add_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();

    if(upperCaseButton)
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "#"));
    else
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "+"));

    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);
}

void Keyboard::on_add_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-#_2.png");
        QIcon ButtonIcon(pixmap);
        ui->add->setIcon(ButtonIcon);
        ui->add->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-+_2.png");
        QIcon ButtonIcon(pixmap);
        ui->add->setIcon(ButtonIcon);
        ui->add->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_add_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-#.png");
        QIcon ButtonIcon(pixmap);
        ui->add->setIcon(ButtonIcon);
        ui->add->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-+.png");
        QIcon ButtonIcon(pixmap);
        ui->add->setIcon(ButtonIcon);
        ui->add->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_tanhao_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();

    if(upperCaseButton)
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "{"));
    else
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "!"));

    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);
}

void Keyboard::on_tanhao_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-{_2.png");
        QIcon ButtonIcon(pixmap);
        ui->tanhao->setIcon(ButtonIcon);
        ui->tanhao->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-!_2.png");
        QIcon ButtonIcon(pixmap);
        ui->tanhao->setIcon(ButtonIcon);
        ui->tanhao->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_tanhao_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-{.png");
        QIcon ButtonIcon(pixmap);
        ui->tanhao->setIcon(ButtonIcon);
        ui->tanhao->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-!.png");
        QIcon ButtonIcon(pixmap);
        ui->tanhao->setIcon(ButtonIcon);
        ui->tanhao->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_midParenthese_clicked()
{
    cursorPos = ui->keyboardLineEdit->cursorPosition();

    if(upperCaseButton)
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "]"));
    else
        ui->keyboardLineEdit->setText(ui->keyboardLineEdit->text().insert(cursorPos, "["));

    ui->keyboardLineEdit->setCursorPosition(cursorPos + 1);
}

void Keyboard::on_midParenthese_pressed()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-]_2.png");
        QIcon ButtonIcon(pixmap);
        ui->midParenthese->setIcon(ButtonIcon);
        ui->midParenthese->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-[_2.png");
        QIcon ButtonIcon(pixmap);
        ui->midParenthese->setIcon(ButtonIcon);
        ui->midParenthese->setIconSize(pixmap.rect().size());
    }
}

void Keyboard::on_midParenthese_released()
{
    if(upperCaseButton){
        QPixmap pixmap(":/images/icon-].png");
        QIcon ButtonIcon(pixmap);
        ui->midParenthese->setIcon(ButtonIcon);
        ui->midParenthese->setIconSize(pixmap.rect().size());
    }else{
        QPixmap pixmap(":/images/icon-[.png");
        QIcon ButtonIcon(pixmap);
        ui->midParenthese->setIcon(ButtonIcon);
        ui->midParenthese->setIconSize(pixmap.rect().size());
    }
}
