//
// Created by yichu on 2026/3/23.
//

#ifndef AI_HISTORY_MYWIDGET_H
#define AI_HISTORY_MYWIDGET_H

#include "mainwindow.h"
#include "message_struct.h"

class MyWidget : public QWidget {
    Q_OBJECT
public:
    MyWidget(QWidget *parent = nullptr);
    std::set<message_struct> will_print;
    // void mktime_line();
protected:
    void paintEvent(QPaintEvent *event) override;
    //QSize sizeHint() const override;
};


#endif //AI_HISTORY_MYWIDGET_H