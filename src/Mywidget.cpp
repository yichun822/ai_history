//
// Created by yichu on 2026/3/23.
//

#include "Mywidget.h"
#include "QWidget"
#include "QPainter"
#include "QPen"
#include "QBrush"

MyWidget::MyWidget(QWidget *parent) : QWidget(parent) {
    this->resize(0,130);
}

void MyWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //qDebug()<<"paint";
    QPen pen(Qt::black,2);
    painter.setPen(pen);

    painter.drawLine(0,this->height()/2,this->width(),this->height()/2);
    auto p=will_print.begin();
    for (int i = 0; i < will_print.size(); i++) {
        QRect c(250*i+10,0,230,this->height()/2-8);
        QRect t(250*i+10,this->height()/2+5,230,20);
        QRect h(250*i+10,this->height()/2+30,230,20);
        painter.drawText(c, Qt::AlignBottom | Qt::AlignHCenter | Qt::TextWordWrap, p->content);
        painter.drawText(t,Qt::AlignHCenter,QString::number(p->time));
        painter.drawText(h,Qt::AlignHCenter | Qt::TextWordWrap,p->happened);
        ++p;
        painter.drawLine(250*i+125,this->height()/2-3,250*i+125,this->height()/2);
    }
}
// void MyWidget::mktime_line() {
//     update();
// }