#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "deepseekclient.h"
#include <QDebug>
#include <set>
MyWidget *mywidget;
DeepSeekClient *deepseek_ai;
std::set<message_struct> messages;

//简易的状态管理器
enum {
    Read,
    Time,
    Happened,
    Content
}mode=Read;


int str_to_int(const QString& str) {
    int res = 0;
    for (char i: str.toStdString()) {
        if (i-'0'>9||i-'0'<0)return -1;
        res *= 10;
        res += i-'0';
    }
    return res;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mywidget = new MyWidget;
    deepseek_ai = new DeepSeekClient(this);
    connect(ui->pushButton_2,&QPushButton::clicked,this,[this] {
        deepseek_ai->setApiKey(ui->lineEdit->text());
        deepseek_ai->sendMessage(ui->textEdit->toPlainText());
        ui->textEdit->clear();
    });
    connect(deepseek_ai,&DeepSeekClient::responseReceived,this,[this](const QString &response) {
        //数据处理，shit写法，实在不想优化了
        QString str = "";message_struct *p=new message_struct;QString time="";
        mode = Read;
        std::string f;  //字符串缓冲区
        bool n=true; //正负号缓冲区
        for (char i: response.toStdString()) {
            switch (mode){
                case Read:
                    if (i==':') {
                        if (str == "time")mode = Time,str="";
                        else if (str == "content")mode = Content,str="";
                        else if (str == "happened")mode = Happened,str="";
                    }
                    else if (str == "EOF") {
                        if (p->content!= "")messages.insert(*p);
                        str="";delete p;
                        p=new message_struct; //这里理论上是能够确保p被delete的，但不知道为啥LSP会报警告
                    }
                    else str+=i;
                    break;
                case Content:
                    if (i==';'){
                        mode=Read;
                        p->content =QString::fromStdString(f) ;//缝缝补补写法，可以改，但能用就得了
                        f="";
                        break;
                    }
                    f+=i;
                    break;
                case Happened:
                    if (i == ';') {
                        p->happened=QString::fromStdString(f);
                        f="";
                        mode = Read;
                        break;
                    }
                    f+=i;
                    break;
                case Time:
                    if (i=='-')n=false;
                    else if (i == ';') {
                        mode = Read;
                        p->time = str_to_int(time);
                        if (false == n)p->time=0-p->time;
                        n=true;
                        if (p->time == -1)goto error; //这里用个goto作为错误跳出，不要太在意了
                        time="";
                        break;
                    }
                    else time+=i;
                    break;
            }
        }
        error:
        delete p;
        qDebug()<<response;
        emit message_changed();
    });
    connect(this, &MainWindow::message_changed, this, &MainWindow::draw_history_times);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mywidget;
    delete deepseek_ai;
}

void MainWindow::draw_history_times() {
    delete mywidget;
    mywidget = new MyWidget;
    mywidget->resize(static_cast<int>(messages.size())*250,ui->scrollArea->height());
    ui -> scrollArea->setWidget(mywidget);
    mywidget->will_print=messages;
    qDebug()<<mywidget->size();
    update();

}

void MainWindow::paintEvent(QPaintEvent *event) {
    mywidget->resize(static_cast<int>(messages.size())*250,ui->scrollArea->height());
}
