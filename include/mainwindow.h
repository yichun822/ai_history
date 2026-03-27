#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Mywidget.h"
#include "message_struct.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void message_changed();
private slots:
    void draw_history_times();
private:
    Ui::MainWindow *ui;
protected:
    void paintEvent(QPaintEvent *event) override;
};
#endif // MAINWINDOW_H
