#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>

//Если очень не хочется писать "uint8_t const *", use TU8PTR
//typedef uint8_t const * TU8PTR;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void GuiUpdateCallback(uint8_t const *arrptr, size_t);

private:
    Ui::MainWindow *ui;
    QThread *thread;
};

class Worker : public QObject
{
    Q_OBJECT

public:
    QTimer *timerDeviceRead;
    Worker();

public slots:
    void updateUSBDataCallback();

signals:
    void GuiUpdatePlease(uint8_t const *arrptr,size_t);
};

#endif // MAINWINDOW_H
