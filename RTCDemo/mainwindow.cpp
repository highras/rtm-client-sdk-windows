#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDateTime>
#include <RTMGateQuestProcessor.h>
#include <RTCGateQuestProcessor.h>
#include <FPLog.h>
#include <QIntValidator>
#include <md5.h>
#include <hex.h>
#include <functional>
#include <AudioRecorder.h>
#include <AudioPlayer.h>
#include <future>

extern bool running;
extern MainWindow* g_mainWindow;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &str)
{
    QString text;
    text.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
    text.append(" ");
    text.append(str);
    g_mainWindow->emitPrintLogs(text);
}

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent),
	ui(new Ui::MainWindow),
	rtc(nullptr),
    srv(-1),
    eventHandler(new MyEventHandler())
{
	ui->setupUi(this);
    for (int i = 0; i != 5; i++)
    {
        videoWidgets[i] = nullptr;
    }
    setWindowTitle("RTCDemo");
    ui->uid->setValidator(new QIntValidator(0, 1000000000));
    ui->rid->setValidator(new QIntValidator(0, 10000));

    connect(ui->initButton, SIGNAL(clicked()), this, SLOT(init()));
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(login()));
    connect(ui->createRoomButton, SIGNAL(clicked()), this, SLOT(createRoom()));
	connect(ui->enterRoomButton, SIGNAL(clicked()), this, SLOT(enterRoom()));
	connect(ui->subscribeButton, SIGNAL(clicked()), this, SLOT(subscribe()));
    connect(ui->unsubscribeButton, SIGNAL(clicked()), this, SLOT(unsubscribe()));
    connect(ui->exitRoomButton, SIGNAL(clicked()), this, SLOT(exitRoom()));
    connect(this,SIGNAL(printLogs(QString&)),this, SLOT(onPrintLogs(QString&)));

    g_mainWindow = this;
    qInstallMessageHandler(messageHandler);
}

MainWindow::~MainWindow()
{
	running = false;
    delete ui;
}

void MainWindow::emitPrintLogs(QString &logs)
{
    emit printLogs(logs);
}

void MainWindow::init()
{
    srv = ui->serverComboBox->currentIndex();
    switch (srv)
    {
    case 0:
        pid = 11000002;
        rtc.reset(new RTCProxy("161.189.171.91", 13321, pid, ui->uid->text().toLongLong(), nullptr,"161.189.171.91", 13702, eventHandler));
        sgClient = TCPClient::createClient("161.189.171.91", 13315);
        secret = "f5a45c68-2279-4de7-b00e-aa10287531a8";
        break;
    case 1:
        pid = 80000071;
        rtc.reset(new RTCProxy("rtm-nx-front.ilivedata.com", 13321, pid, ui->uid->text().toLongLong(), nullptr, "rtc-nx-front.ilivedata.com", 13702, eventHandler));
        sgClient = TCPClient::createClient("rtm-nx-back.ilivedata.com", 13315);
        secret = "bdaf683c-c007-4baa-91aa-1456ebb8c7f1";
        break;
    case 2:
        pid = 80000087;
        rtc.reset(new RTCProxy("rtm-intl-frontgate.ilivedata.com", 13321, pid, ui->uid->text().toLongLong(), nullptr, "rtc-intl-frontgate.ilivedata.com", 13702, eventHandler));
        sgClient = TCPClient::createClient("rtm-intl-backgate.ilivedata.com", 13315);
        secret = "94c04aa3-9a12-42aa-864d-1e8928a86ded";
        break;
    }
    ui->serverComboBox->setEnabled(false);
    ui->loginButton->setEnabled(true);
}

void MainWindow::login()
{
    int32_t ts = slack_real_sec();
    int64_t salt = slack_real_msec();
    string content = to_string(pid) + ":" + secret + ":" + to_string(salt) + ":" + "gettoken" + ":" + to_string(ts);
    unsigned char digest[16];
    md5_checksum(digest, content.c_str(), content.size());
    char hexstr[32 + 1];
    Hexlify(hexstr, digest, sizeof(digest));
    string sign = string(hexstr);

    promise<bool> prm;
    future<bool> f = prm.get_future();

    FPQWriter qw(5, "gettoken");
    qw.param("pid", pid);
    qw.param("sign", sign);
    qw.param("salt", salt);
    qw.param("ts", ts);
    qw.param("uid", ui->uid->text().toLongLong());
    string token;
    sgClient->sendQuest(qw.take(), [this, &prm](FPAnswerPtr answer, int errorCode) {
        if (errorCode != 0)
        {
            qDebug() << "get uid token failed: ";
            qDebug() << "answer: " << answer->json().c_str();
            return;
        }
        else
        {
            FPAReader ar(answer);
            string token = ar.getString("token");
            rtc->Login([this, &prm](int pid, int uid, int errorCode, bool successed) {
                prm.set_value(successed);
            }, token);
        }
        });
    bool successed = f.get();
    if (successed)
    {
        ui->createRoomButton->setEnabled(true);
        ui->enterRoomButton->setEnabled(true);
        currentRid = ui->rid->text().toLongLong();
        currentUid = ui->uid->text().toLongLong();
        ui->uid->setEnabled(false);
        ui->rid->setEnabled(false);
    }
    else
    {
        qDebug() << "rtm auth failed";
    }
}

void MainWindow::createRoom()
{
    promise<bool> prm;
    future<bool> f = prm.get_future();
    rtc->CreateRTCRoom(2, ui->rid->text().toLongLong(), 0, [&prm](int errorCode, bool microphone){
        if (errorCode == 0)
        {
            prm.set_value(true);
        }
        else
        {
            qDebug() << "create room failed error code:"<< errorCode;
            prm.set_value(false);
        }
    });
    bool successed = f.get();
    if (successed)
    {
        ui->subscribeButton->setEnabled(true);
        ui->unsubscribeButton->setEnabled(true);
        ui->exitRoomButton->setEnabled(true);
        rtc->StartAudio();
    }
}


void MainWindow::enterRoom()
{
    promise<bool> prm;
    future<bool> f = prm.get_future();
    rtc->EnterRTCRoom(ui->rid->text().toLongLong(), [&prm](int errorCode, bool microphone) {
        if (errorCode == 0)
        {
            prm.set_value(true);
        }
        else
        {
            qDebug() << "enter room failed error code:" << errorCode;
            prm.set_value(false);
        }
        });
    bool successed = f.get();
    if (successed)
    {
        ui->subscribeButton->setEnabled(true);
        ui->unsubscribeButton->setEnabled(true);
        ui->exitRoomButton->setEnabled(true);
        rtc->StartAudio();
    }
}

void MainWindow::exitRoom()
{
    promise<bool> prm;
    future<bool> f = prm.get_future();
    rtc->ExitRTCRoom([&prm](int errorCode) {
        if (errorCode != 0)
        {
            prm.set_value(false);
        }
        else
        {
            prm.set_value(true);
        }
        });
    if (f.get())
    {
        ui->subscribeButton->setEnabled(false);
        ui->unsubscribeButton->setEnabled(false);
        ui->exitRoomButton->setEnabled(false);
        rtc->StopAudio();
    }
}

void MainWindow::subscribe()
{
    for (int i = 0; i != 5; i++)
    {
        if (videoWidgets[i] == nullptr)
        {
            promise<bool> prm;
            future<bool> f = prm.get_future();
            QWidget* rendererOutput = new QWidget(ui->videoContainer->widget());
            rendererOutput->setFixedWidth(240);
            rendererOutput->setFixedHeight(320);
            rtc->SubscribeVideo(ui->rid->text().toLongLong(), ui->suid->text().toLongLong(),(HWND)rendererOutput->winId(),240,320, [&prm](int errorCode) {
                if (errorCode != 0)
                {
                    prm.set_value(false);
                }
                else
                {
                    prm.set_value(true);
                }
                });
            if (f.get())
            {
                ui->videoContainer->addWidget(rendererOutput, 0, i, Qt::AlignLeading);
                videoWidgets[i] = rendererOutput;
                userMaps.insert(make_pair(ui->suid->text().toLongLong(), i));
            }
            return;
        }
    }

    qDebug() << "cannot subscribe more than 5 video, please unsubscribe one.";
}

void MainWindow::unsubscribe()
{
    promise<bool> prm;
    future<bool> f = prm.get_future();
    rtc->UnsubscribeVideo(ui->rid->text().toLongLong(), ui->suid->text().toLongLong(), [&prm](int errorCode) {
        if (errorCode != 0)
        {
            prm.set_value(false);
        }
        else
        {
            prm.set_value(true);
        }
        });
    if (f.get())
    {
        auto iter = userMaps.find(ui->suid->text().toLongLong());
        if (iter != userMaps.end())
        {
            ui->videoContainer->removeWidget(videoWidgets[iter->second]);
            delete videoWidgets[iter->second];
            videoWidgets[iter->second] = nullptr;
            userMaps.erase(iter);
        }
    }
}

void MainWindow::onPrintLogs(QString &logs)
{
    ui->textBrowser->append(logs);
}

