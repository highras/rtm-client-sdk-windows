#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <thread>
#include <RTMClient.h>
#include <RTCClient.h>
#include "QDX12Renderer.h"
#include "RTCProxy.h"
#include "MyEventHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void emitPrintLogs(QString &logs);

private slots:
    void init();
    void login();
    void createRoom();
    void enterRoom();
    void exitRoom();
    void subscribe();
    void unsubscribe();

    void onPrintLogs(QString &logs);

signals:
    void printLogs(QString &logs);

private:
    Ui::MainWindow *ui;
    shared_ptr<RTCProxy> rtc;
    TCPClientPtr sgClient;
    int64_t currentRid;
    int64_t currentUid;

    string secret;
    int64_t pid;
    int32_t srv;
    shared_ptr<MyEventHandler> eventHandler;

    unordered_map<int64_t, int16_t> userMaps;
    QWidget* videoWidgets[5];
};
#endif // MAINWINDOW_H
