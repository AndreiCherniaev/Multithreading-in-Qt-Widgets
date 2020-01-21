//  Author      : Черняев Андрей
//  Description : Многопоточное приложение с передачей данных из нити обработки в нить формы по указателю
#include "mainwindow.h"
#include "ui_mainwindow.h"

Worker::Worker(){
    qRegisterMetaType<QSharedPointer<uint8_t>>("QSharedPointer<uint8_t>");  //костыль https://stackoverflow.com/questions/58770096/sending-signal-with-a-size-t-variable-from-a-thread-is-not-received-in-the-other
    qRegisterMetaType<std::size_t>("size_t");  //костыль https://stackoverflow.com/questions/58770096/sending-signal-with-a-size-t-variable-from-a-thread-is-not-received-in-the-other
    qRegisterMetaType<uint8_t const *>("uint8_t const *");  //костыль https://stackoverflow.com/questions/58770096/sending-signal-with-a-size-t-variable-from-a-thread-is-not-received-in-the-other

    this->timerDeviceRead = new QTimer();
    connect(Worker::timerDeviceRead, &QTimer::timeout, this, &Worker::updateUSBDataCallback);
    this->timerDeviceRead->start();
}

void Worker::updateUSBDataCallback(){
    size_t mysize = 65;
    uint8_t buf[65] = { "I like slow USB HID" };
    emit GuiUpdatePlease(buf,mysize);
    //Этот код окажет влияние при типе соединения QueuedConnection между GuiUpdatePlease и GuiUpdateCallback
    //вызывая грязное чтения
    for(int i =0;i<64;i++){
        buf[i]=i+'0';
        if (i+'0' > 250) i=0;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    thread = new QThread;
    Worker *worker = new Worker;
    worker->moveToThread(thread);
    //Попробуйте вариант соединения Qt::QueuedConnection
    connect(worker, &Worker::GuiUpdatePlease, this, &MainWindow::GuiUpdateCallback, Qt::BlockingQueuedConnection);
    thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GuiUpdateCallback(uint8_t const *arrptr, size_t length){
    for(volatile uint64_t i=0;i<20000000UL;i++);  //имитируем задержкой, что arrptr очень сложно и долго выводится в GUI (обостряем грязное чтение)
    char const *ptr = reinterpret_cast<char const *>(arrptr);
    QString tmpQStr= QString::fromLocal8Bit(ptr);
    ui->textEdit->setText(tmpQStr);
}
