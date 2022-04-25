#include <qdebug.h>
#include <future>
#include <chrono>
#include <ctime> 
#include <ratio>
#include <functional>
#include <QScrollArea>  
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "md5.h"
#include "hex.h"
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , eventhandler(new MyEventHandler())
{
    ui->setupUi(this);
    ui->serverComboBox->addItem("test");
    ui->serverComboBox->addItem("nx");
    ui->serverComboBox->addItem("intl");
}

MainWindow::~MainWindow()
{
    delete ui;
}

int64_t MainWindow::getNowTime()
{
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

void MainWindow::pressLoginTest(std::string token)
{
    int cnt = 10000;
    for (int i = 0; i < cnt; ++i) {
        vecLoginThreads.push_back(std::thread([this, token, i]() {
            this->rtm->Login([](int pid, int uid, int errorCode, bool successed) {
                if (successed)
                {
                    std::cout << "Login success, pid:" << pid << ", uid:" << uid << std::endl;
                }
                else
                {
                    std::cout << "Login failed, errorCode:" << errorCode << std::endl;
                }
                }, token);
         }));
    }

    for (int i = 0; i < cnt; ++i)
        vecLoginThreads[i].join();
}

void MainWindow::pressClientTest()
{
    int cnt = 10000;
    for (int i = 0; i < cnt; ++i) {
        vecClientThreads.push_back(std::thread([this]() {
            shared_ptr<RTMProxy> rtm1 = std::make_shared<RTMProxy>("161.189.171.91", 13321, pid, ui->uidLineEdit->text().toLongLong(), eventhandler);
            }));
    }

    for (int i = 0; i < cnt; ++i)
        vecClientThreads[i].join();
}

void MainWindow::on_Login_clicked()
{
    if (rtm.use_count() == 0)
    {
        pid = 11000002;
        secret = "f5a45c68-2279-4de7-b00e-aa10287531a8";
        rtm.reset(new RTMProxy("161.189.171.91", 13321, pid, ui->uidLineEdit->text().toLongLong(), eventhandler));
        tcpclient = TCPClient::createClient("161.189.171.91", 13315);
        //pressClientTest();
    }

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
    qw.param("uid", ui->uidLineEdit->text().toLongLong());
    string token;
    tcpclient->sendQuest(qw.take(), [this, &prm](FPAnswerPtr answer, int errorCode) {
        if (answer)
        {
            if (errorCode != 0)
            {
                qDebug() << "get uid token failed: ";
                qDebug() << "answer: " << answer->json().c_str();
                prm.set_value(true);
                return;
            }
            else
            {
                FPAReader ar(answer);
                string token = ar.getString("token");
                //pressLoginTest(token);
                
                rtm->Login([this, &prm](int pid, int uid, int errorCode, bool successed) {
                    if (successed)
                    {
                        qDebug() << "Login success, pid:" << pid << "uid:" << uid;
                    }
                    else
                    {
                        qDebug() << "Login failed, errorCode:" << errorCode;
                    }
                    prm.set_value(true);
                }, token);
            }
        }
        else
        {
            qDebug() << "Gettoken failed, errorCode answer empty!";
            prm.set_value(true);
        }
    });
    f.get();
}


void MainWindow::on_Logout_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();

    rtm->Bye([&prom](int errorCode) {
        if (errorCode != 0)
        {
            qDebug() << "Logout failed, errorCode:" << errorCode;
        }
        else
        {
            qDebug() << "Logout success...";
        }
        prom.set_value(true);
    });

    f.get();
}

void MainWindow::on_SendChat_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t to = 102;
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    int64_t mid = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    string msg = "hello";
    string attrs = "chat";
    int64_t omtime = 0;
    rtm->SendChat(to, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendChat failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendChat success, mtime:" << omtime << ", mid:" << mid;
    }
}


void MainWindow::on_SendCmd_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t to = 102;
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    int64_t mid = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    string msg = "hello";
    string attrs = "cmd";
    int64_t omtime = 0;
    rtm->SendCmd(to, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendCmd failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendCmd success, mtime:" << omtime << ", mid:" << mid;
    }
}


void MainWindow::on_SendRoomChat_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    auto duration = std::chrono::system_clock::now().time_since_epoch();
    int64_t mid = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    string msg = "hello";
    string attrs = "{\"chat1\":\"room\"}";
    int64_t omtime = 0;
    rtm->SendRoomChat(ui->ridLineEdit->text().toLongLong(), mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendRoomChat failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendRoomChat success, mtime:" << omtime;
    }
}


void MainWindow::on_SendRoomCmd_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    auto duration = std::chrono::system_clock::now().time_since_epoch();
    int64_t mid = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    string msg = "hello";
    string attrs = "{\"cmd1\":\"room\"}";
    int64_t omtime = 0;
    rtm->SendRoomCmd(ui->ridLineEdit->text().toLongLong(), mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendRoomCmd failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendRoomCmd success, mtime:" << omtime;
    }
}


void MainWindow::on_SendGroupChat_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    auto duration = std::chrono::system_clock::now().time_since_epoch();
    int64_t mid = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    string msg = "hello";
    string attrs = "{\"chat\":\"group\"}";
    int64_t omtime = 0;
    rtm->SendGroupChat(1000, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendGroupChat failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendGroupChat success, mtime:" << omtime;
    }
}


void MainWindow::on_SendGroupCmd_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    auto duration = std::chrono::system_clock::now().time_since_epoch();
    int64_t mid = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    string msg = "hello";
    string attrs = "{\"cmd\":\"group\"}";
    int64_t omtime = 0;
    rtm->SendGroupCmd(1000, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendGroupCmd failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendGroupCmd success, mtime:" << omtime;
    }
}


void MainWindow::on_GetChat_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t from = 100;
    int64_t mid = 1646211645129;
    int64_t xid = 102;
    int8_t type = 1;
    int64_t oid = 0;
    int8_t omtype = 0;
    string omsg, oattrs;
    int64_t omtime = 0;
    rtm->GetChat(from, mid, xid, type,
        [&prom, &errcode, &oid, &omtype, &omsg, &oattrs, &omtime](int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oid = id;
            omtype = mtype;
            omsg = msg;
            oattrs = attrs;
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetChat failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetChat success, id:" << oid << ", mtype:" << omtype << ", msg:"
            << QString::fromStdString(omsg) << ", attrs:" << QString::fromStdString(oattrs) << ", mtime:" << omtime;
    }
}


void MainWindow::on_GetGroupChat_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t gid = 1000;
    bool desc = true;
    int num = 5;
    int64_t begin = 1645415800639;
    int64_t end = 1646211962951;
    int64_t lastid = 1646211962951;
    int16_t onum = 5;
    int64_t olastid = 0;
    int64_t obegin = 0;
    int64_t oend = 0;
    vector<CommonMsg> omsgs;
    rtm->GetGroupChat(gid, desc, num, begin, end, lastid,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetGroupChat failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetGroupChat success, num:" << onum;
        for (auto m : omsgs)
        {
            for (int i = 0; i < onum; ++i)
            {
                qDebug() << "msg index:" << i + 1
                    << ", id:" << omsgs[i].id
                    << ", from:" << omsgs[i].from
                    << ", mtype:" << omsgs[i].mtype
                    << ", mid:" << omsgs[i].mid
                    << ", deleted:" << omsgs[i].deleted
                    << ", msg:" << QString::fromStdString(omsgs[i].msg)
                    << ", attrs:" << QString::fromStdString(omsgs[i].attrs)
                    << ", mtime:" << omsgs[i].mtime;
            }
        }
    }
}


void MainWindow::on_GetRoomChat_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t rid = ui->ridLineEdit->text().toLongLong();
    bool desc = true;
    int num = 5;
    int64_t begin = 1645415800639;
    int64_t end = getNowTime();
    int64_t lastid = getNowTime();
    int16_t onum = 5;
    int64_t olastid = 0;
    int64_t obegin = 0;
    int64_t oend = 0;
    vector<CommonMsg> omsgs;
    rtm->GetRoomChat(rid, desc, num, begin, end, lastid,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetRoomChat failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetRoomChat success, num:" << onum;
        for (auto m : omsgs)
        {
            for (int i = 0; i < onum; ++i)
            {
                qDebug() << "msg index:" << i + 1
                    << ", id:" << omsgs[i].id
                    << ", from:" << omsgs[i].from
                    << ", mtype:" << omsgs[i].mtype
                    << ", mid:" << omsgs[i].mid
                    << ", deleted:" << omsgs[i].deleted
                    << ", msg:" << QString::fromStdString(omsgs[i].msg)
                    << ", attrs:" << QString::fromStdString(omsgs[i].attrs)
                    << ", mtime:" << omsgs[i].mtime;
            }
        }
    }
}


void MainWindow::on_GetBroadcastChat_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t rid = ui->ridLineEdit->text().toLongLong();
    bool desc = true;
    int num = 5;
    int64_t begin = 1645415800639;
    int64_t end = 1646212686058;
    int64_t lastid = 1646212686058;
    int16_t onum = 5;
    int64_t olastid = 0;
    int64_t obegin = 0;
    int64_t oend = 0;
    vector<CommonMsg> omsgs;
    rtm->GetBroadcastChat(desc, num, begin, end, lastid,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetBroadcastChat failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetBroadcastChat success, num:" << onum;
        for (int i = 0; i < onum; ++i)
        {
            qDebug() << "msg index:" << i + 1
                << ", id:" << omsgs[i].id
                << ", from:" << omsgs[i].from
                << ", mtype:" << omsgs[i].mtype
                << ", mid:" << omsgs[i].mid
                << ", deleted:" << omsgs[i].deleted
                << ", msg:" << QString::fromStdString(omsgs[i].msg)
                << ", attrs:" << QString::fromStdString(omsgs[i].attrs)
                << ", mtime:" << omsgs[i].mtime;
        }
    }
}


void MainWindow::on_GetP2PChat_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t uid = 102;
    bool desc = true;
    int16_t num = 5;
    int64_t begin = 1645415800639;
    int64_t end = 1646211645129;
    int64_t lastid = 1646211645129;
    int16_t onum = 0;
    int64_t olastid = 0;
    int64_t obegin = 0;
    int64_t oend = 0;
    vector<CommonMsg> omsgs;
    rtm->GetP2PChat(uid, desc, num, begin, end, lastid,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetP2PChat failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetP2PChat success, num:" << onum;
        for (auto m : omsgs)
        {
            for (int i = 0; i < onum; ++i)
            {
                qDebug() << "msg index:" << i + 1
                    << ", id:" << omsgs[i].id
                    << ", from:" << omsgs[i].from
                    << ", mtype:" << omsgs[i].mtype
                    << ", mid:" << omsgs[i].mid
                    << ", deleted:" << omsgs[i].deleted
                    << ", msg:" << QString::fromStdString(omsgs[i].msg)
                    << ", attrs:" << QString::fromStdString(omsgs[i].attrs)
                    << ", mtime:" << omsgs[i].mtime;
            }
        }
    }
}


void MainWindow::on_DelChat_clicked()
{
    if (!rtm)
        return;

    int64_t from = 100;
    int64_t mid = 1646189920271;
    int64_t xid = 102;
    int8_t type = 1;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelMsg(from, mid, xid, type, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "DelChat failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "DelChat success...";
    }
}


void MainWindow::on_SendMsg_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t to = 102;
    auto duration = std::chrono::system_clock::now().time_since_epoch();
    int64_t mid = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    int8_t mtype = 66;
    string msg = "hello";
    string attrs = "zpp";
    int64_t omtime = 0;
    rtm->SendMsg(to, mid, mtype, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendMsg failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendMsg success, mtime:" << omtime << ", mid:" << mid;
    }
}


void MainWindow::on_SendGroupMsg_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    auto duration = std::chrono::system_clock::now().time_since_epoch();
    int64_t mid = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    int8_t mtype = 66;
    string msg = "hello";
    string attrs = "{\"zpp\":\"group\"}";
    int64_t omtime = 0;
    rtm->SendGroupMsg(1000, mid, mtype, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendGroupMsg failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendGroupMsg success, mtime:" << omtime;
    }
}


void MainWindow::on_SendRoomMsg_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    auto duration = std::chrono::system_clock::now().time_since_epoch();
    int64_t mid = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    int8_t mtype = 66;
    string msg = "hello";
    string attrs = "{\"zpp\":\"room\"}";
    int64_t omtime = 0;
    rtm->SendRoomMsg(ui->ridLineEdit->text().toLongLong(), mid, mtype, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendRoomMsg failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendRoomMsg success, mtime:" << omtime;
    }
}


void MainWindow::on_GetMsg_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t from = 100;
    int64_t mid = 1646211438064;
    int64_t xid = 102;  
    int8_t type = 1;
    int64_t oid = 0; 
    int8_t omtype = 0;
    string omsg, oattrs;
    int64_t omtime = 0;
    rtm->GetMsg(from, mid, xid, type,
        [&prom, &errcode, &oid, &omtype, &omsg, &oattrs, &omtime](int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oid = id;
            omtype = mtype;
            omsg = msg;
            oattrs = attrs;
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetMsg failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetMsg success, id:" << oid << ", mtype:" << omtype << ", msg:"
            << QString::fromStdString(omsg) << ", attrs:" << QString::fromStdString(oattrs) << ", mtime:" << omtime;
    }
}


void MainWindow::on_GetGroupMsg_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t gid = 1000;
    bool desc = true;
    int num = 5;
    int64_t begin = 1645415800639;
    int64_t end = 1646191208853;
    int64_t lastid = 1646191208853;
    unordered_set<int8_t> mtypes;
    mtypes.insert(61);
    mtypes.insert(62);
    mtypes.insert(63);
    mtypes.insert(64);
    mtypes.insert(66);
    int16_t onum = 5;
    int64_t olastid = 0; 
    int64_t obegin = 0; 
    int64_t oend = 0; 
    vector<CommonMsg> omsgs;
    rtm->GetGroupMsg(gid, desc, num, begin, end, lastid, mtypes,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetGroupMsg failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetGroupMsg success, num:" << onum;
        for (int i = 0; i < onum; ++i)
        {
            qDebug() << "msg index:" << i + 1
                << ", id:" << omsgs[i].id
                << ", from:" << omsgs[i].from
                << ", mtype:" << omsgs[i].mtype
                << ", mid:" << omsgs[i].mid
                << ", deleted:" << omsgs[i].deleted
                << ", msg:" << QString::fromStdString(omsgs[i].msg)
                << ", attrs:" << QString::fromStdString(omsgs[i].attrs)
                << ", mtime:" << omsgs[i].mtime;
        }
    }
}


void MainWindow::on_GetRoomMsg_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t rid = ui->ridLineEdit->text().toLongLong();
    bool desc = true;
    int num = 5;
    int64_t begin = 1645415800639;
    int64_t end = 1646191045332;
    int64_t lastid = 1646191045332;
    unordered_set<int8_t> mtypes;
    mtypes.insert(61);
    mtypes.insert(62);
    mtypes.insert(63);
    mtypes.insert(64);
    mtypes.insert(66); 
    int16_t onum = 5;
    int64_t olastid = 0;
    int64_t obegin = 0;
    int64_t oend = 0;
    vector<CommonMsg> omsgs;
    rtm->GetRoomMsg(rid, desc, num, begin, end, lastid, mtypes,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetRoomMsg failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetRoomMsg success, num:" << onum;
        for (int i = 0; i < onum; ++i)
        {
            qDebug() << "msg index:" << i + 1
                << ", id:" << omsgs[i].id
                << ", from:" << omsgs[i].from
                << ", mtype:" << omsgs[i].mtype
                << ", mid:" << omsgs[i].mid
                << ", deleted:" << omsgs[i].deleted
                << ", msg:" << QString::fromStdString(omsgs[i].msg)
                << ", attrs:" << QString::fromStdString(omsgs[i].attrs)
                << ", mtime:" << omsgs[i].mtime;
        }
    }
}


void MainWindow::on_GetBroadcastMsg_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t rid = ui->ridLineEdit->text().toLongLong();
    bool desc = true;
    int num = 5;
    int64_t begin = 1645415800639;
    int64_t end = 164620235373316;
    int64_t lastid = 164620235373316;
    unordered_set<int8_t> mtypes;
    mtypes.insert(61);
    mtypes.insert(62);
    mtypes.insert(63);
    mtypes.insert(64);
    mtypes.insert(66);
    int16_t onum = 5;
    int64_t olastid = 0;
    int64_t obegin = 0;
    int64_t oend = 0;
    vector<CommonMsg> omsgs;
    rtm->GetBroadcastMsg(desc, num, begin, end, lastid, mtypes,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetBroadcastMsg failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetBroadcastMsg success, num:" << onum;
        for (int i = 0; i < onum; ++i)
        {
            qDebug() << "msg index:" << i + 1
                << ", id:" << omsgs[i].id
                << ", from:" << omsgs[i].from
                << ", mtype:" << omsgs[i].mtype
                << ", mid:" << omsgs[i].mid
                << ", deleted:" << omsgs[i].deleted
                << ", msg:" << QString::fromStdString(omsgs[i].msg)
                << ", attrs:" << QString::fromStdString(omsgs[i].attrs)
                << ", mtime:" << omsgs[i].mtime;
        }
    }
}


void MainWindow::on_GetP2PMsg_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t uid = 102;
    bool desc = true;
    int16_t num = 5;
    int64_t begin = 1645415800639;
    int64_t end = 1646211188031;
    int64_t lastid = 1646211188031;
    unordered_set<int8_t> mtypes;
    mtypes.insert(61);
    mtypes.insert(62);
    mtypes.insert(63);
    mtypes.insert(64);
    mtypes.insert(66);
    int16_t onum = 0;
    int64_t olastid = 0; 
    int64_t obegin = 0; 
    int64_t oend = 0;
    vector<CommonMsg> omsgs;
    rtm->GetP2PMsg(uid, desc, num, begin, end, lastid, mtypes,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetP2PMsg failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetP2PMsg success, num:" << onum;
        for (auto m : omsgs)
        {
            for (int i = 0; i < onum; ++i)
            {
                qDebug() << "msg index:" << i + 1
                    << ", id:" << omsgs[i].id
                    << ", from:" << omsgs[i].from
                    << ", mtype:" << omsgs[i].mtype
                    << ", mid:" << omsgs[i].mid
                    << ", deleted:" << omsgs[i].deleted
                    << ", msg:" << QString::fromStdString(omsgs[i].msg)
                    << ", attrs:" << QString::fromStdString(omsgs[i].attrs)
                    << ", mtime:" << omsgs[i].mtime;
            }
        }
    }
}


void MainWindow::on_GetSession_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> op2p;
    unordered_set<int64_t> ogroup;
    rtm->GetSession([&prom, &errcode, &op2p, &ogroup](int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            op2p = p2p;
            ogroup = group;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetSession failed, errorCode:" << errcode;
    }
    else
    {
        QString p2pList;
        for (auto p2p : op2p)
        {
            p2pList += QString::number(p2p);
            p2pList += ";";
        }
        QString groupList;
        for (auto group : ogroup)
        {
            groupList += QString::number(group);
            groupList += ";";
        }
        qDebug() << "GetSession success, p2pList:" << p2pList << ", groupList:" << groupList;
    }
}


void MainWindow::on_GetUnread_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    bool clean = false;
    unordered_set<int64_t> op2p;
    unordered_set<int64_t> ogroup;
    rtm->GetUnread(clean,
        [&prom, &errcode, &op2p, &ogroup](int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            op2p = p2p;
            ogroup = group;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetUnread failed, errorCode:" << errcode;
    }
    else
    {
        QString p2pList;
        for (auto p2p : op2p)
        {
            p2pList += QString::number(p2p);
            p2pList += ";";
        }
        QString groupList;
        for (auto group : ogroup)
        {
            groupList += QString::number(group);
            groupList += ";";
        }
        qDebug() << "GetUnread success, p2pList:" << p2pList << ", groupList:" << groupList;
    }
}


void MainWindow::on_GetP2PUnread_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> uids;
    uids.insert(100);
    uids.insert(102);
    int64_t mtime = 1645416272497;
    unordered_set<int8_t> mtypes;
    mtypes.insert(30);
    mtypes.insert(32);
    mtypes.insert(66);
    unordered_map<string, int32_t> op2p;
    unordered_map<string, int64_t> oltime;
    rtm->GetP2PUnread(uids, mtime, mtypes,
        [&prom, &errcode, &op2p, &oltime](int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            op2p = p2p;
            oltime = ltime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetP2PUnread failed, errorCode:" << errcode;
    }
    else
    {
        QString infoList1, infoList2;
        auto iter = op2p.begin();
        for (; iter != op2p.end(); ++iter)
        {
            infoList1 += QString::fromStdString(iter->first);
            infoList1 += ";";

            infoList2 += QString::number(iter->second);
            infoList2 += ";";
        }
        qDebug() << "GetP2PUnread success, p2p key:" << infoList1 << ", p2p value:" << infoList2;

        QString infoList3, infoList4;
        auto iter2 = oltime.begin();
        for (; iter2 != oltime.end(); ++iter2)
        {
            infoList3 += QString::fromStdString(iter2->first);
            infoList3 += ";";

            infoList4 += QString::number(iter2->second);
            infoList4 += ";";
        }
        qDebug() << "GetP2PUnread success, ltime key:" << infoList3 << ", ltime value:" << infoList4;
    }
}


void MainWindow::on_GetGroupUnread_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;
    
    unordered_set<int64_t> gids;
    gids.insert(1000);
    int64_t mtime = 1645416272497;
    unordered_set<int8_t> mtypes;
    mtypes.insert(66);
    unordered_map<string, int32_t> ogroup;
    unordered_map<string, int64_t> oltime;
    rtm->GetGroupUnread(gids, mtime, mtypes,
        [&prom, &errcode, &ogroup, &oltime](int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ogroup = group;
            oltime = ltime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetGroupUnread failed, errorCode:" << errcode;
    }
    else
    {
        QString infoList1, infoList2;
        unordered_map<string, int32_t>::iterator iter = ogroup.begin();
        for (; iter != ogroup.end(); ++iter)
        {
            infoList1 += QString::fromStdString(iter->first);
            infoList1 += ";";

            infoList2 += QString::number(iter->second);
            infoList2 += ";";
        }
        qDebug() << "GetGroupUnread success, group key:" << infoList1 << ", group value:" << infoList2;

        QString infoList3, infoList4;
        auto iter2 = oltime.begin();
        for (; iter2 != oltime.end(); ++iter2)
        {
            infoList3 += QString::fromStdString(iter2->first);
            infoList3 += ";";

            infoList4 += QString::number(iter2->second);
            infoList4 += ";";
        }
        qDebug() << "GetGroupUnread success, ltime key:" << infoList3 << ", ltime value:" << infoList4;
    }
}


void MainWindow::on_GetP2PUnreadByTime_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_map<string, int64_t> uidTime;
    uidTime.emplace("102", 1645416272497);
    unordered_set<int8_t> mtypes;
    mtypes.insert(30);
    mtypes.insert(32);
    mtypes.insert(66);
    unordered_map<string, int32_t> op2p;
    unordered_map<string, int64_t> oltime;
    rtm->GetP2PUnreadByTime(uidTime, mtypes,
        [&prom, &errcode, &op2p, &oltime](int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            op2p = p2p;
            oltime = ltime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetP2PUnreadByTime failed, errorCode:" << errcode;
    }
    else
    {
        QString infoList1, infoList2;
        unordered_map<string, int32_t>::iterator iter = op2p.begin();
        for (; iter != op2p.end(); ++iter)
        {
            infoList1 += QString::fromStdString(iter->first);
            infoList1 += ";";

            infoList2 += QString::number(iter->second);
            infoList2 += ";";
        }
        qDebug() << "GetP2PUnreadByTime success, p2p key:" << infoList1 << ", p2p value:" << infoList2;

        QString infoList3, infoList4;
        unordered_map<string, int64_t>::iterator iter2 = oltime.begin();
        for (; iter2 != oltime.end(); ++iter2)
        {
            infoList3 += QString::fromStdString(iter2->first);
            infoList3 += ";";

            infoList4 += QString::number(iter2->second);
            infoList4 += ";";
        }
        qDebug() << "GetP2PUnreadByTime success, ltime key:" << infoList3 << ", ltime value:" << infoList4;
    }
}


void MainWindow::on_GetGroupUnreadByTime_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string gid = "1000";
    unordered_map<string, int64_t> gidTime;
    gidTime.emplace(gid, 1645416272497);
    unordered_set<int8_t> mtypes;
    mtypes.insert(66);
    unordered_map<string, int32_t> op2p;
    unordered_map<string, int64_t> oltime;
    rtm->GetGroupUnreadByTime(gidTime, mtypes,
        [&prom, &errcode, &op2p, &oltime](int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            op2p = p2p;
            oltime = ltime;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetGroupUnreadByTime failed, errorCode:" << errcode;
    }
    else
    {
        QString infoList1, infoList2;
        auto iter = op2p.begin();
        for (; iter != op2p.end(); ++iter)
        {
            infoList1 += QString::fromStdString(iter->first);
            infoList1 += ";";

            infoList2 += QString::number(iter->second);
            infoList2 += ";";
        }
        qDebug() << "GetGroupUnreadByTime success, p2p key:" << infoList1 << ", p2p value:" << infoList2;

        QString infoList3, infoList4;
        unordered_map<string, int64_t>::iterator iter2 = oltime.begin();
        for (; iter2 != oltime.end(); ++iter2)
        {
            infoList3 += QString::fromStdString(iter2->first);
            infoList3 += ";";

            infoList4 += QString::number(iter2->second);
            infoList4 += ";";
        }
        qDebug() << "GetGroupUnreadByTime success, ltime key:" << infoList3 << ", ltime value:" << infoList4;
    }
}


void MainWindow::on_DelMsg_clicked()
{
    if (!rtm)
        return;

    int64_t from = 100;
    int64_t mid = 1646189920271;
    int64_t xid = 102;
    int8_t type = 1;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelMsg(from, mid, xid, type, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "DelMsg failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "DelMsg success...";
    }
}


void MainWindow::on_CleanTargetUnread_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> uids;
    unordered_set<int64_t> gids;
    uids.insert(102);
    gids.insert(1000);
    rtm->CleanTargetUnread(uids, gids,
        [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "CleanTargetUnread failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "CleanTargetUnread success...";
    }
}


void MainWindow::on_OneKeyForMsg_clicked()
{

}


void MainWindow::on_EnterRoom_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->EnterRoom(ui->ridLineEdit->text().toLongLong(), [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    if (errcode != 0)
    {
        qDebug() << "EnterRoom failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "EnterRoom success, rid:" << ui->ridLineEdit->text().toLongLong();
    }
}


void MainWindow::on_LeaveRoom_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->LeaveRoom(ui->ridLineEdit->text().toLongLong(), [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    if (errcode != 0)
    {
        qDebug() << "LeaveRoom failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "LeaveRoom success, rid:" << ui->ridLineEdit->text().toLongLong();
    }
}


void MainWindow::on_GetUserRooms_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;
    unordered_set<int64_t> orooms;

    rtm->GetUserRooms([&prom, &errcode, &orooms](int errorCode, unordered_set<int64_t> rooms) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            orooms = rooms;
        }
        prom.set_value(true);
    });

    f.get();
    if (errcode != 0)
    {
        qDebug() << "GetUserRooms failed, errorCode:" << errcode;
    }
    else
    {
        QString roomList;
        for (auto room: orooms)
        {
            roomList += QString::number(room);
            roomList += ";";
        }
        qDebug() << "GetUserRooms success, roomList:" << roomList;
    }
}


void MainWindow::on_SetRoomInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string oinfo = "hello";
    string pinfo = "world";
    rtm->SetRoomInfo(ui->ridLineEdit->text().toLongLong(), oinfo, pinfo, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    if (errcode != 0)
    {
        qDebug() << "SetRoomInfo failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SetRoomInfo success...";
    }
}


void MainWindow::on_GetRoomInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string oinfo, opinfo;
    rtm->GetRoomInfo(ui->ridLineEdit->text().toLongLong(), [&prom, &errcode, &oinfo, &opinfo](int errorCode, string info, string pinfo) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
            opinfo = pinfo;
        }
        prom.set_value(true);
    });

    f.get();
    if (errcode != 0)
    {
        qDebug() << "GetRoomInfo failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetRoomInfo success, oinfo:" << QString::fromStdString(oinfo) << ", opinfo:" << QString::fromStdString(opinfo);
    }
}


void MainWindow::on_GetRoomOpenInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string ooinfo;
    rtm->GetRoomOpenInfo(ui->ridLineEdit->text().toLongLong(), [&prom, &errcode, &ooinfo](int errorCode, string oinfo) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ooinfo = oinfo;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetRoomOpenInfo failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetRoomOpenInfo success, oinfo:" << QString::fromStdString(ooinfo);
    }
}


void MainWindow::on_GetRoomsOpenInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> rids; 
    rids.insert(ui->ridLineEdit->text().toLongLong());
    unordered_map<string, string> oinfo;
    rtm->GetRoomsOpenInfo(rids, [&prom, &errcode, &oinfo](int errorCode, unordered_map<string, string> info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();
    if (errcode != 0)
    {
        qDebug() << "GetRoomsOpenInfo failed, errorCode:" << errcode;
    }
    else
    {
        QString infoList1, infoList2;
        unordered_map<string, string>::iterator iter = oinfo.begin();
        for (; iter!=oinfo.end(); ++iter)
        {
            infoList1 += QString::fromStdString(iter->first);
            infoList1 += ";";

            infoList2 += QString::fromStdString(iter->second);
            infoList2 += ";";
        }
        qDebug() << "GetRoomsOpenInfo success, infoList1:" << infoList1 << ", infoList2:" << infoList2;
    }
}


void MainWindow::on_GetRoomMemers_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> ouids;
    rtm->GetRoomMemers(ui->ridLineEdit->text().toLongLong(), [&prom, &errcode, &ouids](int errorCode, unordered_set<int64_t> uids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetRoomMemers failed, errorCode:" << errcode;
    }
    else
    {
        QString uidList;
        for (auto room : ouids)
        {
            uidList += QString::number(room);
            uidList += ";";
        }
        qDebug() << "GetRoomMemers success, uidList:" << uidList;
    }
}


void MainWindow::on_GetRoomCount_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> rids;
    rids.insert(ui->ridLineEdit->text().toLongLong());
    unordered_map<string, int32_t> oinfo;
    rtm->GetRoomCount(rids, [&prom, &errcode, &oinfo](int errorCode, unordered_map<string, int32_t> info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetRoomCount failed, errorCode:" << errcode;
    }
    else
    {
        QString infoList1, infoList2;
        unordered_map<string, int32_t>::iterator iter = oinfo.begin();
        for (; iter != oinfo.end(); ++iter)
        {
            infoList1 += QString::fromStdString(iter->first);
            infoList1 += ";";

            infoList2 += QString::number(iter->second);
            infoList2 += ";";
        }
        qDebug() << "GetRoomCount success, infoList1:" << infoList1 << ", infoList2:" << infoList2;
    }
}


void MainWindow::on_GetOnlineUsers_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> uids;
    uids.insert(ui->uidLineEdit->text().toLongLong());
    unordered_set<int64_t> ouids;
    rtm->GetOnlineUsers(uids,
        [&prom, &errcode, &ouids](int errorCode, unordered_set<int64_t> uids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetOnlineUsers failed, errorCode:" << errcode;
    }
    else
    {
        QString ouidList;
        for (auto uid : ouids)
        {
            ouidList += QString::number(uid);
            ouidList += ";";
        }
        qDebug() << "GetOnlineUsers success, uidList:" << ouidList;
    }
}


void MainWindow::on_SetUserInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string oinfo = "set";
    string pinfo = "userinfo";
    rtm->SetUserInfo(oinfo, pinfo,
        [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SetUserInfo failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SetUserInfo success...";
    }
}


void MainWindow::on_GetUserInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string oinfo, opinfo;
    rtm->GetUserInfo([&prom, &errcode, &oinfo, &opinfo](int errorCode, string info, string pinfo) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
            opinfo = pinfo;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetUserInfo failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetUserInfo success, oinfo:" << QString::fromStdString(oinfo) << ", opinfo:" << QString::fromStdString(opinfo);
    }
}


void MainWindow::on_GetUserOpenInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> uids;
    uids.insert(ui->uidLineEdit->text().toLongLong());
    unordered_map<string, string> oinfo;
    rtm->GetUserOpenInfo(uids, [&prom, &errcode, &oinfo](int errorCode, unordered_map<string, string> info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetUserOpenInfo failed, errorCode:" << errcode;
    }
    else
    {
        QString infoList1, infoList2;
        unordered_map<string, string>::iterator iter = oinfo.begin();
        for (; iter != oinfo.end(); ++iter)
        {
            infoList1 += QString::fromStdString(iter->first);
            infoList1 += ";";

            infoList2 += QString::fromStdString(iter->second);
            infoList2 += ";";
        }
        qDebug() << "GetUserOpenInfo success, infoList1:" << infoList1 << ", infoList2:" << infoList2;
    }
}


void MainWindow::on_AddFriends_clicked()
{
    if (!rtm)
        return;

    unordered_set<int64_t> friends;
    for (int i=101; i<=120; ++i)
        friends.insert(i);

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->AddFriends(friends, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "AddFriends failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "AddFriends success...";
    }
}


void MainWindow::on_DelFriends_clicked()
{
    if (!rtm)
        return;

    unordered_set<int64_t> friends;
    friends.insert(101);
    friends.insert(103);

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelFriends(friends, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "DelFriends failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "DelFriends success...";
    }
}


void MainWindow::on_GetFriends_clicked()
{
    if (!rtm)
        return;

    unordered_set<int64_t> ouids;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetFriends([&prom, &errcode, &ouids](int errorCode, unordered_set<int64_t> uids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetFriends failed, errorCode:" << errcode;
    }
    else
    {
        QString ouidList;
        for (auto uid : ouids)
        {
            ouidList += QString::number(uid);
            ouidList += ";";
        }
        qDebug() << "GetFriends success, uidList:" << ouidList;
    }
}


void MainWindow::on_AddBlacks_clicked()
{
    if (!rtm)
        return;

    unordered_set<int64_t> blacks;
    for (int i = 101; i <= 105; ++i)
        blacks.insert(i);

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->AddBlacks(blacks, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "AddBlacks failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "AddBlacks success...";
    }
}


void MainWindow::on_DelBlacks_clicked()
{
    if (!rtm)
        return;

    unordered_set<int64_t> blacks;
    blacks.insert(101);
    blacks.insert(103);

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelBlacks(blacks, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "DelBlacks failed, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "DelBlacks success...";
    }
}


void MainWindow::on_GetBlacks_clicked()
{
    if (!rtm)
        return;

    unordered_set<int64_t> ouids;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetBlacks([&prom, &errcode, &ouids](int errorCode, unordered_set<int64_t> uids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetBlacks failed, errorCode:" << errcode;
    }
    else
    {
        QString ouidList;
        for (auto uid : ouids)
        {
            ouidList += QString::number(uid);
            ouidList += ";";
        }
        qDebug() << "GetBlacks success, ouidList:" << ouidList;
    }
}


void MainWindow::on_AddGroupMembers_clicked()
{
    if (!rtm)
        return;

    int64_t gid = 1000;
    unordered_set<int64_t> uids;
    for (int i = 200; i <= 220; ++i)
        uids.insert(i);

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->AddGroupMembers(gid, uids, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "AddGroupMembers fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "AddGroupMembers success...";
    }
}


void MainWindow::on_DelGroupMembers_clicked()
{
    if (!rtm)
        return;

    int64_t gid = 1000;
    unordered_set<int64_t> uids;
    for (int i = 201; i <= 205; ++i)
        uids.insert(i);

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelGroupMembers(gid, uids, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "DelGroupMembers fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "DelGroupMembers success...";
    }
}


void MainWindow::on_GetGroupMembers_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t gid = 1000;
    bool online = true;
    unordered_set<int64_t> ouids;
    unordered_set<int64_t> oonlines;
    rtm->GetGroupMembers(gid, online, [&prom, &errcode, &ouids, &oonlines](int errorCode, unordered_set<int64_t> uids, unordered_set<int64_t> onlines) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
            oonlines = onlines;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "DelGroupMembers failed, errorCode:" << errcode;
    }
    else
    {
        QString ouidList;
        for (auto uid : ouids)
        {
            ouidList += QString::number(uid);
            ouidList += ";";
        }

        QString onlines;
        for (auto online : oonlines)
        {
            onlines += QString::number(online);
            onlines += ";";
        }
        qDebug() << "DelGroupMembers success, onlines:" << onlines;
    }
}


void MainWindow::on_GetGroupCount_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t gid = 1000;
    bool online = true;
    int32_t oocn = 0, oonlines = 0;
    rtm->GetGroupCount(gid, online, [&prom, &errcode, &oocn, &oonlines](int errorCode, int32_t ocn, int32_t onlines) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oocn = ocn;
            oonlines = onlines;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetGroupCount fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetGroupCount success, ocn:" << oocn << ", onlines : " << oonlines;
    }
}


void MainWindow::on_GetUserGroups_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> ogids;
    rtm->GetUserGroups([&prom, &errcode, &ogids](int errorCode, unordered_set<int64_t> gids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ogids = gids;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetUserGroups failed, errorCode:" << errcode;
    }
    else
    {
        QString gidList;
        for (auto gid : ogids)
        {
            gidList += QString::number(gid);
            gidList += ";";
        }

        qDebug() << "GetUserGroups success, gidList:" << gidList;
    }
}


void MainWindow::on_SetGroupInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int gid = 1000;
    string oinfo = "gaojiawei";
    string pinfo = "is bad";
    rtm->SetGroupInfo(gid, oinfo, pinfo, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SetGroupInfo fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SetGroupInfo success...";
    }
}


void MainWindow::on_GetGroupOpenInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int gid = 1000;
    string oinfo;
    rtm->GetGroupOpenInfo(gid, [&prom, &errcode, &oinfo](int errorCode, string info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetGroupOpenInfo fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetGroupOpenInfo success, oinfo:" << QString::fromStdString(oinfo);
    }
}


void MainWindow::on_GetGroupsOpenInfo_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_set<int64_t> gids;
    gids.insert(1000);
    unordered_map<string, string> oinfo;
    rtm->GetGroupsOpenInfo(gids, [&prom, &errcode, &oinfo](int errorCode, unordered_map<string, string> info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetGroupsOpenInfo failed, errorCode:" << errcode;
    }
    else
    {
        QString infoList1, infoList2;
        unordered_map<string, string>::iterator iter = oinfo.begin();
        for (; iter != oinfo.end(); ++iter)
        {
            infoList1 += QString::fromStdString(iter->first);
            infoList1 += ";";

            infoList2 += QString::fromStdString(iter->second);
            infoList2 += ";";
        }
        qDebug() << "GetGroupsOpenInfo success, infoList1:" << infoList1 << ", infoList2:" << infoList2;
    }
}


void MainWindow::on_OneKeyForRoomOrGroupOrFriend_clicked()
{

}


void MainWindow::on_AddAttrs_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_map<string, string> attrs;
    attrs.insert(std::pair<string, string>("1", "a"));
    attrs.insert(std::pair<string, string>("2", "b"));
    attrs.insert(std::pair<string, string>("3", "c"));
    rtm->AddAttrs(attrs, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "AddAttrs fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "AddAttrs success...";
    }
}


void MainWindow::on_GetAttrs_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    unordered_map<string, string> oattrs;
    rtm->GetAttrs([&prom, &errcode, &oattrs](int errorCode, unordered_map<string, string> attrs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oattrs = attrs;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetAttrs failed, errorCode:" << errcode;
    }
    else
    {
        QString infoList1, infoList2;
        unordered_map<string, string>::iterator iter = oattrs.begin();
        for (; iter != oattrs.end(); ++iter)
        {
            infoList1 += QString::fromStdString(iter->first);
            infoList1 += ";";

            infoList2 += QString::fromStdString(iter->second);
            infoList2 += ";";
        }
        qDebug() << "GetAttrs success, oattrs key:" << infoList1 << ", oattrs value:" << infoList2;
    }
}


void MainWindow::on_AddDebugLog_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string msg = "c:/";
    string attrs = "t.txt";
    rtm->AddDebugLog(msg, attrs, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "AddDebugLog fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "AddDebugLog success...";
    }
}


void MainWindow::on_GetServerTime_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    int64_t  omts = 0;
    rtm->GetServerTime([&prom, &errcode, &omts](int errorCode, int64_t mts) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omts = mts;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "GetServerTime fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "GetServerTime success, mts:" << omts;
    }
}

void MainWindow::on_StartRecord_clicked()
{
    if (!rtm)
        return;

    rtm->StartRecordAudio();
}


void MainWindow::on_StopRecord_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;
    int32_t mtime = 0;

    rtm->StopRecordAudio(rtmAudio);
    {
        rtm->SendFile([&errcode, &prom, &mtime](int32_t ec, int64_t mts)
            {
                errcode = ec;
                if (errcode == 0)
                {
                    mtime = mts;
                }
                prom.set_value(true);
            }, 120, rtmAudio);
    }

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SendFile fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SendFile success...";
    }
}


void MainWindow::on_StartPlayer_clicked()
{
    if (!rtm)
        return;

    rtm->StartPlayAudio(rtmAudio);
}


void MainWindow::on_DataDel_2_clicked()
{
    if (!rtm)
        return;

    rtm->StopPlayAudio();
}

void MainWindow::on_FileToken_clicked()
{
    /* if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string cmd = "sendfile";
    int64_t to = 102;
    int64_t rid = 100;
    int64_t gid = 1000;
    string otoken;
    string oendpoint;
    rtm->FileToken(cmd, to, rid, gid,
        [&prom, &errcode, &otoken, &oendpoint](int errorCode, string token, string endpoint) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            otoken = token;
            oendpoint = endpoint;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "FileToken fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "FileToken success, token:" << QString::fromStdString(otoken) << ", endpoint:" << QString::fromStdString(oendpoint);
    }*/
}


void MainWindow::on_DataGet_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string key = "dataKey";
    string oval;
    rtm->DataGet(key, [&prom, &errcode, &oval](int errorCode, string val) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oval = val;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "DataGet fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "DataGet success, val:" << QString::fromStdString(oval);
    }
}


void MainWindow::on_DataSet_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string key = "dataKey";
    string val = "dataValue";
    rtm->DataSet(key, val, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "DataSet fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "DataSet success...";
    }
}


void MainWindow::on_DataDel_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string key = "dataKey";
    rtm->DataDel(key, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "DataDel fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "DataDel success...";
    }
}


void MainWindow::on_OneKeyForFileOrUserSystem_clicked()
{

}


void MainWindow::on_SetLang_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string lang = "ge";
    rtm->SetLang(lang, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "SetLang fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "SetLang success...";
    }
}


void MainWindow::on_Translate_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string text = "hello world";
    string src = "en";
    string dst = "en";  
    string type;
    string profanity;
    string osource, otarget, osourceText, otargetText;
    rtm->Translate(text, src, dst, type, profanity,
        [&prom, &errcode, &osource, &otarget, &osourceText, &otargetText](int errorCode, string source, string target, string sourceText, string targetText) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            osource = source;
            otarget = target;
            osourceText = sourceText;
            otargetText = targetText;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "Translate fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "Translate success, source:" << QString::fromStdString(osource) << ", target:" << QString::fromStdString(otarget) 
            << ", sourceText:" << QString::fromStdString(osourceText) << ", targetText:" << QString::fromStdString(otargetText);
    }
}


void MainWindow::on_Profanity_clicked()
{
    if (!rtm)
        return;

    string text = "son of bitch";
    bool classify = false;
    string otext;
    vector<string> oclassification;
    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->Profanity(text, classify,
        [&prom, &errcode, &otext, &oclassification](int errorCode, string text, vector<string> classification) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            otext = text;
            oclassification = oclassification;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "Profanity fail, errorCode:" << errcode;
    }
    else
    {
        QString classfList;
        for (auto classf : oclassification)
        {
            classfList += QString::fromStdString(classf);
            classfList += ";";
        }
        qDebug() << "Profanity success, text:" << QString::fromStdString(otext) << ", classfList:" << classfList;
    }
}

#include <iostream>
#include <fstream>
#include <string>
void MainWindow::on_Speech2Text_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string audio;
    std::ifstream file("C:/res/raw/test.amr", std::ios::in | std::ios::ate | std::ios::binary);
    if (file) {
        int filesize = file.tellg();
        audio.reserve(filesize);

        file.seekg(0);
        while (!file.eof())
        {
            audio += file.get();
        }
    }
    cout << "audio file length:" << audio.length() << endl;
    
    int32_t type = 2;
    string lang = "zh-cn";
    string codec = "AMR_WB";
    int32_t srate = 16000;
    string otext;  
    string olang;
    rtm->Speech2Text(audio, type, lang, codec, srate,
        [&prom, &errcode, &otext, &olang](int errorCode, string text, string lang) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            otext = text;
            olang = lang;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "Speech2Text fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "Speech2Text success, text:" << QString::fromStdString(otext) << ", lang:" << QString::fromStdString(olang);
    }
}


void MainWindow::on_TCheck_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string text = "fuck";
    int32_t oresult = 0;
    string otext; 
    vector<string> otags;
    vector<string> owlist;
    rtm->TCheck(text,
        [&prom, &errcode, &oresult, &otext, &otags, &owlist](int errorCode, int32_t result, string text, vector<string> tags, vector<string> wlist) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oresult = result;
            otext = text;
            otags = tags;
            owlist = wlist;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "TCheck fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "TCheck success(0-pass,2-unpass), result:" << oresult;
    }
}


void MainWindow::on_ICheck_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string image = "C:/res/mipmap-hdpi/ic_launcher.png";
    int32_t type = 1;
    int32_t oresult = 0;
    vector<string> otags;
    rtm->ICheck(image, type,
        [&prom, &errcode, &oresult, &otags](int errorCode, int32_t result, vector<string> tags) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oresult = result;
            otags = tags;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "ICheck fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "ICheck success(0-pass,2-unpass), result:" << oresult;
    }
}


void MainWindow::on_ACheck_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string audio = "C:/res/raw/demo.amr";;
    int32_t type = 1; 
    string lang = "ch";
    string codec = "AMR_WB";
    int32_t srate = 16000;
    int32_t oresult = 0;
    vector<string> otags;
    rtm->ACheck(audio, type, lang, codec, srate,
        [&prom, &errcode, &oresult, &otags](int errorCode, int32_t result, vector<string> tags) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oresult = result;
            otags = tags;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "ACheck fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "ACheck success(0-pass,2-unpass), result:" << oresult;
    }
}


void MainWindow::on_VCheck_clicked()
{
    if (!rtm)
        return;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    string video = "C:/res/video/Sticker.mp4";
    int32_t type = 1; 
    string videoName;
    int32_t oresult = 0; 
    vector<string> otags;
    rtm->VCheck(video, type, videoName,
        [&prom, &errcode, &oresult, &otags](int errorCode, int32_t result, vector<string> tags) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oresult = result;
            otags = tags;
        }
        prom.set_value(true);
    });

    f.get();

    if (errcode != 0)
    {
        qDebug() << "VCheck fail, errorCode:" << errcode;
    }
    else
    {
        qDebug() << "VCheck success(0-pass,2-unpass), result:" << oresult;
    }
}


void MainWindow::on_OneKeyForUpgradeService_clicked()
{

}




