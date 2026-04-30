#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "deepseekclient.h"
#include <QDebug>
#include <set>
#include<QJsonObject>
#include<QJsonArray>
#include<QJsonDocument>
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
        QByteArray byteArray=response.toUtf8();
        QJsonParseError error;
        QJsonDocument document = QJsonDocument::fromJson(byteArray,&error);
        if (error.error != QJsonParseError::NoError && !document.isArray()) {
            goto error;
        }
        else {
            QJsonArray message_array = document.array();
            for (int i=0;i<message_array.size();i++) {
                QJsonObject message_object=message_array[i].toObject();
                p->time = message_object["time"].toString();
                p->happened = message_object["happened"].toString();
                p->content = message_object["content"].toString();
                messages.insert(*p);
                delete p;
                p=new message_struct;
            }
            delete p;
        }

        error:
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
