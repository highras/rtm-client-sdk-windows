#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "fpnn.h"
#include "RTMProxy.h"
#include "MyEventHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using namespace fpnn;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    shared_ptr<RTMProxy> rtm;
    TCPClientPtr tcpclient;
    shared_ptr<MyEventHandler> eventhandler;
    std::vector<std::thread> vecLoginThreads;
    std::vector<std::thread> vecClientThreads;

    string secret;
    int64_t pid;
    int64_t uid;
    int64_t rid;

    RTMAudio rtmAudio;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int64_t getNowTime();
    void pressLoginTest(std::string token);
    void pressClientTest();

private slots:
    void on_Login_clicked();

    void on_Logout_clicked();

    void on_SendChat_clicked();

    void on_SendCmd_clicked();

    void on_SendRoomChat_clicked();

    void on_SendRoomCmd_clicked();

    void on_SendGroupChat_clicked();

    void on_SendGroupCmd_clicked();

    void on_GetChat_clicked();

    void on_GetGroupChat_clicked();

    void on_GetRoomChat_clicked();

    void on_GetBroadcastChat_clicked();

    void on_GetP2PChat_clicked();

    void on_DelChat_clicked();

    void on_SendMsg_clicked();

    void on_SendGroupMsg_clicked();

    void on_SendRoomMsg_clicked();

    void on_GetMsg_clicked();

    void on_GetGroupMsg_clicked();

    void on_GetRoomMsg_clicked();

    void on_GetBroadcastMsg_clicked();

    void on_GetP2PMsg_clicked();

    void on_GetSession_clicked();

    void on_GetUnread_clicked();

    void on_GetP2PUnread_clicked();

    void on_GetGroupUnread_clicked();

    void on_GetP2PUnreadByTime_clicked();

    void on_GetGroupUnreadByTime_clicked();

    void on_DelMsg_clicked();

    void on_CleanTargetUnread_clicked();

    void on_OneKeyForMsg_clicked();

    void on_EnterRoom_clicked();

    void on_LeaveRoom_clicked();

    void on_GetUserRooms_clicked();

    void on_SetRoomInfo_clicked();

    void on_GetRoomInfo_clicked();

    void on_GetRoomOpenInfo_clicked();

    void on_GetRoomsOpenInfo_clicked();

    void on_GetRoomMemers_clicked();

    void on_GetRoomCount_clicked();

    void on_GetOnlineUsers_clicked();

    void on_SetUserInfo_clicked();

    void on_GetUserInfo_clicked();

    void on_GetUserOpenInfo_clicked();

    void on_AddFriends_clicked();

    void on_DelFriends_clicked();

    void on_GetFriends_clicked();

    void on_AddBlacks_clicked();

    void on_DelBlacks_clicked();

    void on_GetBlacks_clicked();

    void on_AddGroupMembers_clicked();

    void on_DelGroupMembers_clicked();

    void on_GetGroupMembers_clicked();

    void on_GetGroupCount_clicked();

    void on_GetUserGroups_clicked();

    void on_SetGroupInfo_clicked();

    void on_GetGroupOpenInfo_clicked();

    void on_GetGroupsOpenInfo_clicked();

    void on_OneKeyForRoomOrGroupOrFriend_clicked();

    void on_AddAttrs_clicked();

    void on_GetAttrs_clicked();

    void on_AddDebugLog_clicked();

    void on_GetServerTime_clicked();

    void on_StartRecord_clicked();

    void on_StopRecord_clicked();

    void on_StartPlayer_clicked();

    void on_DataDel_2_clicked();

    void on_FileToken_clicked();

    void on_DataGet_clicked();

    void on_DataSet_clicked();

    void on_DataDel_clicked();

    void on_OneKeyForFileOrUserSystem_clicked();

    void on_SetLang_clicked();

    void on_Translate_clicked();

    void on_Profanity_clicked();

    void on_Speech2Text_clicked();

    void on_TCheck_clicked();

    void on_ICheck_clicked();

    void on_ACheck_clicked();

    void on_VCheck_clicked();

    void on_OneKeyForUpgradeService_clicked();

};
#endif // MAINWINDOW_H
